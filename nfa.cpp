#include "nfa.h"

#define GENERATE_CODE
#define TEMPLTYPE statep
int cmp_statep(statep a, statep b){ return (a>b)?1:(a<b?-1:0); }
#include "vector.h"
#undef TEMPLTYPE

/* NFA-based matcher, per http://swtch.com/~rsc/regexp/regexp1.html */

int listid = 0;

State *elList2NFA(elList *lst)
{
  Fragment *nfa = NULL;
  bool end_anchor = false;
  
  elNode *node = lst->head;
  
  if(node->t == STR_START)
    node = node->next;
  else
    frag_accumulate(&nfa, frag_ANY_CHAR(ZERO_OR_MORE));
  
  while(node != NULL) {
    switch(node->t) {
      case STR_START:
        printf("problem: reached START node somewhere besides the beginning.\n");
        goto NFAError;
      case STR_END:
        end_anchor = true;
        break;
      case ANY_CHAR:
        frag_accumulate(&nfa, frag_ANY_CHAR(node->r));
        break;
      case LITERAL:
        frag_accumulate(&nfa, frag_LITERAL(node->ch, node->r, node->neg));
        break;
      case CHARSET:
        frag_accumulate(&nfa, frag_CHARSET(node->chs, node->r, node->neg));
        break;
    }
    node = node->next;
  }
  if(!end_anchor)
    frag_accumulate(&nfa, frag_ANY_CHAR(ZERO_OR_MORE));

  frag_accumulate(&nfa, frag_match());
  
  if(debug)printStates(nfa->start, 0);
  return nfa->start;
  
 NFAError:
  /* <<FIXME>> Cleanup Allocated Memory */
  return NULL;
}

void addState(statepVec *v, State *s)
{
  if(s == NULL || s->lastlist == listid)
    return;
  
  s->lastlist = listid;

  if(s->t == Split) {
    addState(v, s->out1);
    addState(v, s->out2);
  }
  else {
    push_statepVec(v, s);
  }
}

void stepNFA(statepVec *currs, char c, statepVec **nexts)
{ 
  unsigned int i, j;
  State *s;
  
  if(debug)printf("stepNFA: analyzing currs:\n");
  if(debug)printStateList(currs);
  
  *nexts = new_statepVec();   listid++;
  for(i = 0; i < currs->len; i++) {
    s = currs->vec[i];
    //if(debug)printf("checking state for match: ");
    //if(debug)printState(s);
    switch(s->t) {
      case Literal:
        if(s->neg)
        {
          bool found = false;
          for(j = 0; j < s->chs->len; j++)
            if(s->chs->vec[j] == c) {
              found = true;
              break;
            }
          if(!found)
            addState(*nexts, s->out1);
          break;
        }
        else
        {
          for(j = 0; j < s->chs->len; j++)
            if(s->chs->vec[j] == c) {
              addState(*nexts, s->out1);
              break;
            }
          break;
        }
      case AnyChar:
        addState(*nexts, s->out1);
        break;
      case Matched:
        break;
      case Split:
        addState(*nexts, s->out1);
        addState(*nexts, s->out2);      
        break;
      default:
        printf("unknown state type reached! bailing...\n");
        return;
        break;
    }
  }
}

bool match_aux(State *start, string *txt)
{
  statepVec *currs, *nexts;
  //static int c = 0;
  bool ret;
  unsigned int i;
  
  currs = new_statepVec();  listid++;
  addState(currs, start);
  
  for(i = 0 ; i < txt->length(); i++) {
    //if(debug)printf("step %d: '%c'\n", c++, *txt);
    stepNFA(currs, (*txt)[i], &nexts);
    //if(debug)printf("step completed; deleting old list\n");
    del_statepVec(currs);
    currs = nexts;
  }
  
  ret = isMatch(currs);
  del_statepVec(currs);
  return ret;
}
