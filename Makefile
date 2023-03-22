
CC=gcc
CFLAGS=-Wall
LDFLAGS=-lncurses

OBJS=obj/main.c.o
BIN=minesweeper.out

.PHONY: clean all run

all: $(BIN)

run: $(BIN)
	./$<

# -------------------------------------------

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): obj/%.c.o : src/%.c
	@mkdir -p obj/
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

# -------------------------------------------

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

