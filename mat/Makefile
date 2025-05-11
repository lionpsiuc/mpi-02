CC      = mpicc
CFLAGS  = -I./include -O3 -Wall -Wextra
LDFLAGS = -lm

SRCDIR   = src
BUILDDIR = build
BINDIR   = bin

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

EXECS = $(BINDIR)/main

$(shell mkdir -p $(BUILDDIR) $(BINDIR))

all: $(EXECS)

$(BINDIR)/main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean run

clean:
	$(RM) -r $(BUILDDIR)/* $(BINDIR)/*

run: $(EXECS)
	mpirun -np 4 $(BINDIR)/main
