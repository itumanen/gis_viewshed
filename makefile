CC=g++
CFLAGS=-I

DEPS = grid.h fa_grid.h fd_grid.h
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: viewshed.cpp
	$(CC) -o viewshed viewshed.cpp grid.cpp fa_grid.cpp fd_grid.cpp