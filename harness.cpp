#include "common.h"
#include "nfa.h"
#include "regex.h"

int debug = false;

string exprs[] = {NULL, /* a?^na^n will be generated in main rather than be typed out */
                 "a", 
                 "abc", 
                 "^a$", 
                 "[a-c]", 
                 "^[^a]$", 
                 "^[^a]?$", 
                 "a+", 
                 ".", 
                 ".*", 
                 ".+", 
                 "da?[bd]"
                };
int n_exprs = sizeof(exprs)/sizeof(exprs[0]);

string texts[] = {NULL, /* a^n will be generated in main rather than be typed out */
                 "", 
                 "a", 
                 "h", 
                 "dd"
                 "abc", 
                 "dad", 
                 "dabcd", 
                 "aaaaaab", 
                };
int n_texts = sizeof(texts)/sizeof(texts[0]);

int main(void)
{
  string *re, *txt;
  int i, e, t, nNodes;
  elList *lst;
  State *start;
  bool badre;
  
  regex_t regex;
  int reti;
  
  printf("getting going!\n");
  
  /* generate a?^na^n and a^n for worst-case performance testing */
  int testlen = 100;
  char *strre, *strtxt;
  strre = (char *)malloc(3 * testlen + 1);
  strtxt = (char *)malloc(testlen + 1);
  for(i = 0; i < testlen; i++)
  {
    strre[2 * i] = strre[2 * testlen + i] = strtxt[i] = 'a';
    strre[2 * i + 1] = '?';
  }
  strre[3 * testlen] = strtxt[testlen] = '\0';
  exprs[0] = strre;
  texts[0] = strtxt;
  
  for(e = 0; e < n_exprs; e++)
  {
    bool found, found2;
    re = &exprs[e];
    lst = (elList *)calloc(sizeof(elList), 1);
    badre = false;

    nNodes = parseExp(re, lst);    
    reti = regcomp(&regex, re->c_str(), REG_EXTENDED);
    
    if(reti != 0 || nNodes == 0)
    {
      printf("could not compile \"%s\"; going on to the next\n", re->c_str());
      continue;
    }

    start = elList2NFA(lst);
    
    for(t = 0; t < n_texts; t++)
    {
      txt = &texts[t];
      
      found = match_aux(start, txt);
      found2 = !regexec(&regex, txt->c_str(), 0, NULL, 0);
      
      if(found != found2)
      {
        printf("matching \"%s\" against \"%s\"...\t\t%s\n", re->c_str(), txt->c_str(), found?"MATCHED!":"no match :(");
        printf("(re engine says %s)\n", reti?"no match":"match");
      }
      else 
      {
        printf("%d%d ", found, found2);
      }
    }
    printf("\n");
    free(lst);
    regfree(&regex);
  }
  return 0;
}
