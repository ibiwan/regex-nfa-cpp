#ifndef _PTRLIST_H_
#define _PTRLIST_H_

#include "nfastate.h"

typedef statep* stPtrRef;

#define TEMPLTYPE stPtrRef
#include "vector.h"
#undef TEMPLTYPE

/* stPtrRefVec: Vector of Ptr refs!  Specifically, output state pointers for 
fragments (see fragment.h) that still need to be mapped to a Next state */

extern stPtrRefVec *append(stPtrRefVec *lst1, stPtrRefVec *lst2);
extern void patch(stPtrRefVec *lst, statep s);

#endif /* _PTRLIST_H_ */
