#include "ptrlist.h"

#define GENERATE_CODE
#define TEMPLTYPE stPtrRef
int cmp_stPtrRef(stPtrRef a, stPtrRef b){ return (a>b)?1:(a<b?-1:0); }
#include "vector.h"
#undef TEMPLTYPE

stPtrRefVec *append(stPtrRefVec *lst1, stPtrRefVec *lst2)
{
  stPtrRefVec *lst = new_stPtrRefVec();
  stPtrRef s;
  
  while(pop_stPtrRefVec(lst1, &s))
    push_stPtrRefVec(lst, s);
  while(pop_stPtrRefVec(lst2, &s))
    push_stPtrRefVec(lst, s);
  
  return lst;
}

void patch(stPtrRefVec *lst, statep s)
{
  unsigned int i;
  if(debug)printf("patching %d ptr to:\t", lst->len);
  if(debug)printState(s);
  for(i = 0; i < lst->len; i++) {
    if(debug)printf("patching <%p>...\n", &(lst->vec[i]));
    *(lst->vec[i]) = s;
  }
}
