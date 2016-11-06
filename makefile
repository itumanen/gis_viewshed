CC=g++
CFLAGS=-I

DEPS = grid.h view_grid.h
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: viewshed.cpp
	$(CC) -o viewshed viewshed.cpp grid.cpp view_grid.cpp