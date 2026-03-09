
CFLAGS = -Wall -Wextra -Wshadow -Wunreachable-code -Wredundant-decls  -Wmissing-declarations -Wold-style-definition \
-Wmissing-prototypes -Wdeclaration-after-statement \
-Wno-return-local-addr -Wunsafe-loop-optimizations -Wuninitialized -Werror -Wno-unused-parameter -g3

CC = gcc
PROG1 = ASCIIlosaurus_server
PROG2 = ASCIIlosaurus_client
PROG3 = ASCIIlosaurus_world
PROGS = $(PROG1) $(PROG2)

all: $(PROGS)

$(PROG1): $(PROG1).o
	$(CC) $(CFLAGS) -o $@ $^

$(PROG1).o: $(PROG1).c $(PROG3).h
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG2): $(PROG2).o $(PROG3).o
	$(CC) $(CFLAGS) -o $@ $^ -lncurses

$(PROG2).o: $(PROG2).c $(PROG3).h
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG3).o: $(PROG3).c $(PROG3).h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(PROGS) *~ \#*


