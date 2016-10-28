#include "grid.h"

#define NO_FLOW 0
#define EAST 1
#define SOUTHEAST 2
#define SOUTH 4
#define SOUTHWEST 8
#define WEST 16
#define NORTHWEST 32
#define NORTH 64
#define NORTHEAST 128

class FD_Grid : public Grid {

public:
	FD_Grid(Grid* elevGrid);
	int getDirectionCode(int i, int j);
	int computeFlowDirAt(Grid* elevGrid, int i, int j);
	void computeFlowDir(Grid* elevGrid);

};