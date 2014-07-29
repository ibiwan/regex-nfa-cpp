/* NO SENTINEL IFDEFS; INTENTIONAL! */

#include <stdio.h>

/* USAGE:
#define TEMPLTYPE int
#include "vector.h"
int cmp_int(int a, int b){ return a>b?1:(a<b?-1:0); }
#undef TEMPLTYPE
...
intVec *v = new_intVec();
*/

#define JOIN_AGAIN(x, y)  x ## y // force one layer of macro expansion
#define JOIN_AGAIN2(x, y) JOIN_AGAIN(x, y)  // force two layers

#define TEMPLTYPEVEC(x) JOIN_AGAIN(x, Vec)
#define TVECTYPE        TEMPLTYPEVEC(TEMPLTYPE)
#define NEWFUNC(x)      JOIN_AGAIN2(new_,     TVECTYPE)
#define NEW1FUNC(x)     JOIN_AGAIN2(new1_,    TVECTYPE)
#define DELFUNC(x)      JOIN_AGAIN2(del_,     TVECTYPE)
#define REALLOCF(x)     JOIN_AGAIN2(realloc_, TVECTYPE)
#define PUSHF(x)        JOIN_AGAIN2(push_,    TVECTYPE)
#define POPF(x)         JOIN_AGAIN2(pop_,     TVECTYPE)
#define INSERTF(x)      JOIN_AGAIN2(insert_,  TVECTYPE)
#define HASF(x)         JOIN_AGAIN2(has_,     TVECTYPE)
#define SETADDF(x)      JOIN_AGAIN2(set_add_, TVECTYPE)

#define CMPF(x)         JOIN_AGAIN2(cmp_,     TEMPLTYPE)

#ifndef GENERATE_CODE /* generate headers only */

typedef struct {
  TEMPLTYPE *vec;
  unsigned int len;
  unsigned int max;
} TVECTYPE;

extern TVECTYPE *NEWFUNC( TEMPLTYPE )(void);
extern TVECTYPE *NEW1FUNC( TEMPLTYPE )(TEMPLTYPE t);
extern void DELFUNC( TEMPLTYPE )(TVECTYPE *v);
extern void REALLOCF(TEMPLTYPE)(TVECTYPE *v);
extern void PUSHF(TEMPLTYPE)(TVECTYPE *v, TEMPLTYPE t);
extern bool POPF(TEMPLTYPE)(TVECTYPE *v, TEMPLTYPE *dest);
extern void INSERTF(TVECTYPE)(TVECTYPE *v, TEMPLTYPE t);
extern bool HASF(TEMPLTYPE)(TVECTYPE *v, TEMPLTYPE t);
extern bool SETADDF(TEMPLTYPE)(TVECTYPE *v, TEMPLTYPE t);

#else /*GENERATE_CODE*/

TVECTYPE *NEWFUNC( TEMPLTYPE )(void)
{
  TEMPLTYPEVEC(TEMPLTYPE) *ret = (TVECTYPE *)calloc(sizeof(TVECTYPE), 1);
  ret->max = 8;
  ret->len = 0;
  ret->vec = (TEMPLTYPE *)calloc(sizeof(TEMPLTYPE), ret->max);
  return ret;
}

TVECTYPE *NEW1FUNC( TEMPLTYPE )(TEMPLTYPE t)
{
  TEMPLTYPEVEC( TEMPLTYPE) *ret = NEWFUNC( TEMPLTYPE )();
  PUSHF(TEMPLTYPE)(ret, t);
  return ret;
}

void DELFUNC( TEMPLTYPE )(TVECTYPE *v)
{
  free(v);
}

void REALLOCF(TEMPLTYPE)(TVECTYPE *v)
{
  v->max *= 2;
  v->vec = (TEMPLTYPE *)realloc(v->vec, sizeof(TEMPLTYPE) * v->max);
}

void PUSHF(TEMPLTYPE)(TVECTYPE *v, TEMPLTYPE t)
{
  if(v->len == v->max)
  {
    REALLOCF(TEMPLTYPE)(v);
  }
  v->vec[v->len++] = t;
}

bool POPF(TEMPLTYPE)(TVECTYPE *v, TEMPLTYPE *dest)
{ 
  if(v->len == 0)
    return false;
    
  *dest = v->vec[--v->len];
  return true;
}

void INSERTF(TVECTYPE)(TVECTYPE *v, TEMPLTYPE t)
{
  unsigned int i = 0, j;
  while(i < v->len && CMPF(TMPLTYPE)(v->vec[i], t) < 0)
  {
    i++;
  }
  for(j = v->len; j > i; j--)
  {
    v->vec[j] = v->vec[j-1];
  }
  v->vec[j] = t;
  v->len++;
}

bool HASF(TEMPLTYPE)(TVECTYPE *v, TEMPLTYPE t)
{
  unsigned int i;
  for(i = 0; i < v->len; i++) {
    if(v->vec[i] == t)
      return true;
  }
  return false;  
}


bool SETADDF(TEMPLTYPE)(TVECTYPE *v, TEMPLTYPE t)
{
  if(HASF(TEMPLTYPE)(v, t))
    return false;

  PUSHF(TEMPLTYPE)(v, t);
  return true;
}

#endif /*GENERATE_CODE*/
