#include "grid.h" 

#define VIEW_NOT_COMPUTED -1

class View_Grid : public Grid {
public:
	// Constructor
	View_Grid(Grid* elevGrid);

	// Methods
	void initialize();
	void computeViewshed(Grid* elevGrid, int vp_row, int vp_col);
	int isVisible(Grid* elevGrid, int vp_row, int vp_col, int row, int col);

};

