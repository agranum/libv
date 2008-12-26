CC = gcc
CFLAGS = -W -Wall -ansi -pedantic -Wshadow -Wpointer-arith -Wwrite-strings

ifeq ($(DEBUG), 1)
	CFLAGS += -g -DDEBUG=1
else
	CFLAGS += -O2
endif

SRCS = $(shell ls *.c)
HDRS = $(SRCS:.c=.h)
OBJS = $(SRCS:.c=.o)

.PHONY: clean love

all: $(OBJS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~ core $(PROG) *.exe a.out *.stackdump

love:
	@echo not war.

