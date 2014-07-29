#include "common.h"
#include "nfa.h"

int debug = true;

int main(void)
{
  char *re, *txt;
  int reLen = 100, txtLen = 1000, bytes_read, nNodes;//, nMatches = 0;
  elList *lst = (elList *)calloc(sizeof(elList), 1);
  
  printf("enter a regexp to be parsed.\n");
  re = (char *)calloc(sizeof(char), reLen + 1);

  bytes_read = strlen(fgets(re, reLen, stdin));
  
  if(bytes_read == 0) {
    printf("okay, you want an empty matcher.  you got it.\n(yeah, or you put in a giant (> %d chars) regexp.)\n", reLen);
    return 0;
  }
  
  string res = string(re);
  nNodes = parseExp(&res, lst);

  if(nNodes == 0) {
    return 0;
  }

  State *start = elList2NFA(lst);
  printf("NFA created.\n");

  printf("enter the text to be matched against.\n");
  txt = (char *)calloc(sizeof(char), txtLen + 1);

  bytes_read = strlen(fgets(txt, txtLen, stdin));
  
  if(bytes_read == 0) {
    printf("no text (or too much text, > %d chars), no match!  have a nice day.\n", txtLen);
    return -1;
  }
  
  string txts = string(txt);
  bool found = match_aux(start, &txts);
  if(found)
    printf("matched!\n");
  else
    printf("no match :(\n");
  
  return 0;
}
