/* State: place for the matching state-machine to land. 
Has a StateType to define its behavior. */

#ifndef _NFASTATE_H_
#define _NFASTATE_H_

#include "common.h"

typedef enum {
  Default_Error,
  Literal,
  AnyChar,
  Split,
  Matched,
} StateType;

typedef struct State_t State;

struct State_t {
  StateType t;
  charVec *chs;
  State *out1;
  State *out2;
  bool neg;  // negated match (advance state pointer and text pointer if NOT matched)
  int lastlist;
};

extern State *new_state_n(charVec *v, State *outp1, State *outp2);
extern State *new_state(char c, State *outp1, State *outp2);
extern void printState(State *s);
extern void printStates(State *s, int depth);
extern State *match_state(void);

typedef State* statep;

#define TEMPLTYPE statep
#include "vector.h"
#undef TEMPLTYPE

extern bool isMatch(statepVec *v);
extern void printStateList(statepVec *v);

#endif /*_NFASTATE_H_ */
