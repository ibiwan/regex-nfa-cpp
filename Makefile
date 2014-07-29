OBJS = fragment.o parser.o nfastate.o nfa.o  ptrlist.o
MAIN_OBJS = $(OBJS) main.o
HARN_OBJS = $(OBJS) harness.o
CC = g++
CFLAGS = -g -Wall -Wextra

nfa: $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o nfa $(MAIN_OBJS)

harness: $(HARN_OBJS)
	$(CC) $(CFLAGS) -o harness $(HARN_OBJS)
  
parser.o: parser.h parser.cpp
	$(CC) $(CFLAGS) -c parser.cpp

nfastate.o: nfastate.h nfastate.cpp
	$(CC) $(CFLAGS) -c nfastate.cpp

ptrlist.o: nfastate.h ptrlist.h ptrlist.cpp
	$(CC) $(CFLAGS) -c ptrlist.cpp

fragment.o: parser.h nfastate.h ptrlist.h fragment.h fragment.cpp
	$(CC) $(CFLAGS) -c fragment.cpp

nfa.o: parser.h nfastate.h \
       ptrlist.h fragment.h nfa.h nfa.cpp
	$(CC) $(CFLAGS) -c nfa.cpp

main.o: parser.h nfa.h main.cpp
	$(CC) $(CFLAGS) -c main.cpp

harness.o: parser.h nfa.h harness.cpp
	$(CC) $(CFLAGS) -c harness.cpp

clean:
	rm -f nfa harness *.o core.*


