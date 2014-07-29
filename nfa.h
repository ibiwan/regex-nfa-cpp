#ifndef _NFA_H_
#define _NFA_H_

#include "common.h"
#include "parser.h"
#include "nfastate.h"
#include "ptrlist.h"
#include "fragment.h"

extern State *elList2NFA(elList *lst);
extern bool match_aux(State *start, string *txt);

#endif /*_NFA_H_*/
