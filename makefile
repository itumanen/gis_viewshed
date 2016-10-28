CC=g++
CFLAGS=-I

DEPS = grid.h fa_grid.h fd_grid.h
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: flow.cpp
	$(CC) -o flow flow.cpp grid.cpp fa_grid.cpp fd_grid.cpp