#include "fragment.h"

Fragment *new_fragment(State *s, stPtrRefVec *o)
{
  unsigned int i;
  Fragment *f = (Fragment *)calloc(sizeof(Fragment), 1);
  *f = (Fragment){s, o};
  if(debug)printf("creating Fragment at <%p> with state <%p> and outputs ",
          f, 
          s);
  for(i = 0; i < o->len; i++)
    if(debug)printf("<%p> -> <%p>; ", 
           &(o->vec[i]),
           o->vec[i]);
  if(debug)printf("\b\b\n");
  return f;
}

// >(s)--c-->
Fragment *frag_char(char c, bool negated)              
{
  State *s = new_state(c, NULL, NULL);
  s->t   = Literal;
  s->neg = negated;
  if(debug)printf("creating state:\t\t");
  if(debug)printState(s);
  return new_fragment(s, 
                  new1_stPtrRefVec(&(s->out1)));
}

// >(s)-->
Fragment *frag_any(void)                              
{
  State *s = new_state('\0', NULL, NULL);
  s->t = AnyChar;
  if(debug)printf("creating state:\t\t");
  if(debug)printState(s);
  return new_fragment(s, 
                  new1_stPtrRefVec(&(s->out1)));  
}

// >(s)--[c1|c2|...]-->
Fragment *frag_chars(charVec *v, bool negated)     
{
  State *s = new_state_n(v, NULL, NULL);
  s->t = Literal;
  s->neg = negated;
  if(debug)printf("creating state:\t\t");
  if(debug)printState(s);
  return new_fragment(s, 
                  new1_stPtrRefVec(&(s->out1)));
}

// >[f1]-->[f2]-->
Fragment *frag_concat(Fragment *f1, Fragment *f2)     
{
  if(debug)printf("patching:  f1:\t\t");
  if(debug)printState(f1->start);
  if(debug)printf("           f2:\t\t");
  if(debug)printState(f2->start);
  
  patch(f1->outs, 
        f2->start);

  if(debug)printf("result:\t\t\t");
  if(debug)printState(f1->start);
  
  return new_fragment(f1->start, 
                      f2->outs);
}

//   +-->[f1]-->
// >(s)
//   +-->[f2]-->
Fragment *frag_alternate(Fragment *f1, Fragment *f2)  
{                                                            
  State *s = new_state('\0', f1->start, f2->start);             
  s->t = Split;
  if(debug)printf("creating state:\t");
  if(debug)printState(s);
  return new_fragment(s, 
                  append(f1->outs, 
                         f2->outs));
}

//   +-->[f]-->
// >(s)
//   +-------->
Fragment *frag_zero_one(Fragment *f)                  
{
  Fragment *ret = NULL;
  State *s = new_state('\0', f->start, NULL);                   
  s->t = Split;
  if(debug)printf("create 01 state:\t");
  if(debug)printState(s);
  ret = new_fragment(s,
                 append(f->outs,
                        new1_stPtrRefVec(&(s->out2))));
  if(debug)printf("result:\t\t\t");
  if(debug)printState(ret->start);
  return ret;
}

//   +-->[f]--+
//   |        |
// >(s)<------+
//   |
//   +-------->
inline Fragment *frag_zero_more(Fragment *f)                 
{                                                            
  Fragment *ret = NULL;
  State *s = new_state('\0', f->start, NULL);                   
  s->t = Split;
  if(debug)printf("create 0+ state:\t");
  if(debug)printState(s);
  patch(f->outs, s);                                         
  ret = new_fragment(s,                                         
                 new1_stPtrRefVec(&(s->out2)));
  if(debug)printf("result:\t\t\t");
  if(debug)printState(ret->start);
  return ret;
}

//   +-----+
//   v     |
// >[f]-->(s)-->
Fragment *frag_one_more(Fragment *f)                  
{                                                            
  Fragment *ret = NULL;
  State *s = new_state('\0', f->start, NULL);                   
  s->t = Split;
  if(debug)printf("create 1+ state:\t");
  if(debug)printState(s);
  patch(f->outs, s);
  ret = new_fragment(f->start,
                 new1_stPtrRefVec(&(s->out2)));
  if(debug)printf("result:\t\t\t");
  if(debug)printState(ret->start);
  return ret;
}

Fragment *frag_match(void)
{
  State *s = match_state();
  if(debug)printf("creating state:\t\t");
  if(debug)printState(s);
  return new_fragment(s, 
                  new1_stPtrRefVec(&(s->out1)));  
}


Fragment *frag_rep(Fragment *f, repetition r)
{
  switch(r) {
    case ZERO_OR_ONE:
      return frag_zero_one(f);
    case ZERO_OR_MORE:
      return frag_zero_more(f);
    case ONE_OR_MORE:
      return frag_one_more(f);
    case ONE:           // fallthrough to default
    case NONREPEATABLE: // fallthrough to default
    default:
      return f;      
  }
}

Fragment *frag_LITERAL(char c, repetition r, bool neg)
{
  return frag_rep(frag_char(c, neg), r);
}

Fragment *frag_ANY_CHAR(repetition r)
{
  return frag_rep(frag_any(), r);
}

Fragment *frag_CHARSET(charVec *v, repetition r, int neg)
{
  return frag_rep(frag_chars(v, neg), r);
}

void frag_accumulate(Fragment **acc, Fragment *f)
{
  if(*acc == NULL)
    *acc = f;
  else
    *acc = frag_concat(*acc, f);
}
