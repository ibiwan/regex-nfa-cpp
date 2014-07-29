#include "parser.h"

#define WILDCARD_CHAR     '.'
#define START_CHAR        '^'
#define END_CHAR          '$'
#define ESCAPE_CHAR      '\\'
#define NEGATE_CHAR       '^'
#define REPEAT_0_INF_CHAR '*'
#define REPEAT_1_INF_CHAR '+'
#define REPEAT_0_1_CHAR   '?'
#define START_CLASS_CHAR  '['
#define END_CLASS_CHAR    ']'
#define CLASS_RANGE_CHAR  '-'
#define CR_CHAR          '\r'
#define LF_CHAR          '\n'
#define ESC_TAB_CHAR      't'
#define ESC_CRLF_CHAR     'n'

#define MAX_NODE_STR_LEN 100

typedef enum {
  START,
  BASE,
  END,
  ESCAPE,
  START_CLASS,
  CHAR_CLASS,
  CLASS_ESC,
  CLASS_RANGE,
} parseMode;

void add(elList *lst, elNode *n)
{
  if(lst->count == 0) {
    lst->head = n;
    lst->tail = n;
    n->next = NULL;
  }
  else {
    lst->tail->next = n;
    lst->tail = n;
    n->next = NULL;
  }
  lst->count++;
}

elNode *new_node(void)
{
  elNode *ret = (elNode*)calloc(sizeof(elNode), 1);
  //if(debug)printf("creating elNode <%p>.\n", ret);
  return ret;
}

void deleteElList(elList *lst)
{
  elNode *n;
  while(lst->head != NULL) {
    n = lst->head;
    lst->head = (elNode*)(n->next);
    free(n);
  }
  lst->count = 0;
  lst->head = lst->tail = 0;
}

char escapeChar(char c)
{  
  switch(c) {
    case ESC_TAB_CHAR:  return '\t';
    case ESC_CRLF_CHAR: return '\n'; 
    default:            return c;
  }
}

parseMode mode;
char curr;
bool negclass;
char classlist[UCHAR_MAX];
unsigned int classcount;
elNode *n;
char save;

inline bool processBaseMode(elList **lsta)
{
  switch(curr) {
    case START_CHAR:
      printf("malformed regexp: character[s] ('%c') before START ('%c') token\n", curr, START_CHAR);
      return false;
    case END_CHAR:
      n = new_node();
      n->t = STR_END;
      n->r = NONREPEATABLE;
      add(*lsta, n);
      mode = END;
      break;
    case WILDCARD_CHAR:
      n = new_node();
      n->t = ANY_CHAR;
      add(*lsta, n);
      break;
    case REPEAT_0_INF_CHAR:
      if(n == NULL || n->r == NONREPEATABLE) {
        printf("malformed regexp: repetition character ('%c') must follow a repeatable token\n", REPEAT_0_INF_CHAR);
        return false;
      }
      else {
        n->r = ZERO_OR_MORE;
      }
      break;
    case REPEAT_1_INF_CHAR:
      if(n == NULL || n->r == NONREPEATABLE) {
        printf("malformed regexp: repetition character ('%c') must follow a repeatable token\n", REPEAT_1_INF_CHAR);
        return false;
      }
      else {
        n->r = ONE_OR_MORE;
      }
      break;
    case REPEAT_0_1_CHAR:
      if(n == NULL || n->r == NONREPEATABLE) {
        printf("malformed regexp: repetition character ('%c') must follow a repeatable token\n", REPEAT_0_1_CHAR);
        return false;
      }
      else {
        n->r = ZERO_OR_ONE;
      }
      break;
    case ESCAPE_CHAR:
      mode = ESCAPE;
      break;
    case START_CLASS_CHAR:
      negclass = false;
      classcount = 0;
      mode = START_CLASS;
      break;
    case CR_CHAR:          /* fallthrough to LF_CHAR */
    case LF_CHAR:
      printf("ignoring cr/lf character\n");
      break;
    default:             /* all other characters, just add as literals */
      n = new_node();
      n->t = LITERAL;
      n->ch = curr;
      add(*lsta, n);
      break;
  }
  return true;
}

bool processCharClass(elList **lsta)
{
  unsigned int i;
  switch(curr) {
    case END_CLASS_CHAR:
      n = new_node();
      n->t = CHARSET;
      n->neg = negclass;
      n->chs = new_charVec();
      for(i = 0; i < classcount; i++)
        push_charVec(n->chs, classlist[i]);
      add(*lsta, n);
      mode = BASE;
      break;
    case ESCAPE_CHAR:
      mode = CLASS_ESC;
      break;
    case CLASS_RANGE_CHAR:
      if(save == '\0') {
        printf("malformed regexp: range character ('%c') is first in character class\n", CLASS_RANGE_CHAR);
        return false;
      }
      mode = CLASS_RANGE;
      break;
    default:
      save = curr;
      classlist[classcount++] = save;
      break;
  }
  return true;
}

int parseExp(string *re, elList *lst)
{
  mode = START;
  unsigned int i;
  unsigned char ic;
  
  negclass = false;
  
  n = NULL;
  
  for(i = 0; i < re->length(); i++)
  {
    curr = (*re)[i];
    //printf("parsing, on character '%c'\n", curr);
    
    if(curr < ' ' || curr > '~') {
      printf("malformed regexp: character '%c' not in accepted range\n", curr);
      goto abortParse;
    }
    
    switch(mode)
    {
      case START:
        mode = BASE;
        if(curr == START_CHAR) {
          n = new_node();
          n->t = STR_START;
          n->r = NONREPEATABLE;
          add(lst, n);
          break;
        }
      /* fallthrough to mode==BASE if not START_CHAR */
      case BASE:
        if(!processBaseMode(&lst))
          goto abortParse;
        break;
      case ESCAPE:
        save = escapeChar(curr);
        n = new_node();
        n->t = LITERAL;
        n->ch = curr;
        add(lst,n);
        mode = BASE;
        break;
      case CLASS_ESC:
        save = escapeChar(curr);
        classlist[classcount++] = save;
        mode = CHAR_CLASS;
        break;
      case CLASS_RANGE:
        if(curr == END_CLASS_CHAR) {
          printf("malformed regexp: range \"%c%c\" unfinished before character class closed '%c'\n", save, CLASS_RANGE_CHAR, END_CLASS_CHAR);
          goto abortParse;
        }
        
        if(save < curr && (('a' <= save && curr <= 'z') || 
                           ('A' <= save && curr <= 'Z') || 
                           ('0' <= save && curr <= '9'))) {
          for(ic = save + 1; ic <= curr; ic++) {/* ch already added in previous state */
            classlist[classcount++] = ic;
          }
          mode = CHAR_CLASS;
        }
        else {
          printf("malformed regexp: range cannot be formed from '%c' to '%c'\n", save, curr);
          goto abortParse;
        }
        break;
      case START_CLASS:
        mode = CHAR_CLASS;
        save = '\0';
        if(curr == NEGATE_CHAR) {
          negclass = true;
          break;
        }
      /* fallthrough to mode==CHAR_CLASS if not NEGATE_CHAR */
      case CHAR_CLASS: 
        if(!processCharClass(&lst))
          goto abortParse;
        break;
      case END:
        printf("malformed regexp: characters after END ('%c') token\n", END_CHAR);
        goto abortParse;
    }
  }
  if(mode == START_CLASS || mode == CHAR_CLASS) {
    printf("malformed regexp: ends before character class is closed ('%c')\n", END_CLASS_CHAR);
    goto abortParse;
  }
  if(mode == ESCAPE) {
    printf("malformed regexp: ends on escape character ('%c')\n", ESCAPE_CHAR);
    goto abortParse;
  }
  
  return lst->count;

abortParse:
  deleteElList(lst);
  return 0;
}
