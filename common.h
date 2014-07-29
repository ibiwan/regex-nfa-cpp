#ifndef _COMMON_H_
#define _COMMON_H_

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>

using namespace std;

#define TEMPLTYPE char
#include "vector.h"
#undef TEMPLTYPE

extern int debug;

#endif /* _COMMON_H_ */
