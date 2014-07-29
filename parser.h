#ifndef _PARSER_H_
#define _PARSER_H_

#include "common.h"

typedef enum {
  STR_START,
  STR_END,
  ANY_CHAR,
  LITERAL,
  CHARSET,
} elementType;

typedef enum {
  ONE,
  NONREPEATABLE,
  ZERO_OR_ONE,
  ZERO_OR_MORE,
  ONE_OR_MORE,
} repetition;

typedef struct elNode_t elNode;

struct elNode_t {
  elNode* next;
  elementType t;
  repetition r;
  char ch;
  charVec *chs;
  int neg;
};

typedef struct {
  elNode *head;
  elNode *tail;
  int count;
} elList;

extern int parseExp(string *re, elList *lst);

#endif /*_PARSER_H_*/
