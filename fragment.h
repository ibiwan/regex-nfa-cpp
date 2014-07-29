#ifndef _FRAGMENT_H_
#define _FRAGMENT_H_

#include "common.h"
#include "parser.h"
#include "nfastate.h"
#include "ptrlist.h"

/* Fragment: the Things that are strung together to make the state machine.  
Each fragment is a black box with a starting state and a list of output pointers */

typedef struct Fragment_t Fragment;

struct Fragment_t
{
  State *start;
  stPtrRefVec *outs;
};

extern void frag_accumulate(Fragment **acc, Fragment *f);
extern Fragment *frag_ANY_CHAR(repetition r);
extern Fragment *frag_LITERAL(char c, repetition r, bool neg);
extern Fragment *frag_CHARSET(charVec *v, repetition r, int neg);
extern Fragment *frag_match(void);

#endif /* _FRAGMENT_H_ */
