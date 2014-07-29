#include "nfastate.h"

#define GENERATE_CODE
#define TEMPLTYPE char
int cmp_char(char a, char b){ return (a>b)?1:(a<b?-1:0); }
#include "vector.h"
#undef TEMPLTYPE

State *new_state_n(charVec *v, State *outp1, State *outp2)
{
  State *s = (State *)calloc(sizeof(State), 1);
  *s = (State){Default_Error, 
               v,
               outp1, outp2, 
               false,
               0};
  //if(debug)printf("creating State <%p> with outps stored at <%p>, <%p>\n", 
  //s, &(s->out1), &(s->out2));
  
  return s;  
}

State *new_state(char c, State *outp1, State *outp2)
{
  return new_state_n(new1_charVec(c), outp1, outp2);
}

State *match_state(void)
{
  State *s = new_state('\0', NULL, NULL);
  s->t = Matched;
  return s;
}

void printState(State *s)
{
  unsigned int i;
  State *o1, *o2;
  if(s == NULL)
    printf("NULL state reached.\n");
  else {
    o1 = s->out1;
    o2 = s->out2;
    printf("<%p>\t", s);
    switch(s->t) {
      case Literal:
        printf("Literal state%s: {", 
               s->neg?", Negated":"");
        for(i = 0; i < s->chs->len; i++)
          printf("%c,", s->chs->vec[i]);
        printf("\b}\t<%p> -> <%p>.\n", 
               &(s->out1),
               o1);
        break;
      case AnyChar:
        printf("AnyChar state,\t\t<%p> -> <%p>.\n", 
                &(s->out1),
                o1);
        break;
      case Split:
        printf("Split state,\t\t<%p> -> <%p>; <%p> -> <%p>.\n", 
                &(s->out1),
                o1,
                &(s->out2),
                o2);
        break;
      case Matched:
        printf("Match state reached, wooo!!\n");
        break;
      default:
        printf("Unknown state type.\n");
        break;
    }
  }
}

void printStates(State *s, int depth)
{
  int i;
  static statepVec *log;

  if(depth == 0)
    log = new_statepVec();

  if(s == NULL) {
    for(i = 0; i < depth; i++) printf(" ");  
    printf("NULL\n");
    return;
  }
  
  for(i = 0; i < depth; i++)
    printf(" ");  
  printState(s);

  if(set_add_statepVec(log, s)) {
    // only visit children if parent has not previously been seen
    printStates(s->out1, depth+1);
    printStates(s->out2, depth+1);
  }
}

void printStateList(statepVec *v)
{
  unsigned int i;
  for(i = 0; i < v->len; i++) {
    printf("\t");
    printState(v->vec[i]);
    fflush(stdout);
  }
}

bool isMatch(statepVec *v)
{
  unsigned int i;
  for(i = 0; i < v->len; i++)
  {
    if(v->vec[i]->t == Matched)
    {
      if(debug)printf("MATCH OFFICIALLY FOUND\n");
      return true;
    }
  }
  if(debug)printf("no match yet...\n");
  fflush(stdout);
  return false;
}
