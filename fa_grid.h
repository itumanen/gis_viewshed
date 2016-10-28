#include "fd_grid.h"

class FA_Grid : public Grid {

public:
	FA_Grid(Grid* elevGrid);
	void initializeGrid(Grid* elevGrid);
	float computeFlowAt(Grid* fdGrid, int i, int j);
	void computeFlow(Grid* fdGrid);
	bool flowsInto(Grid* fdGrid, int i, int j, int offsetX, int offsetY);
	void initializeEmptyGrids();

private:

	bool** computedGrid;
	
};