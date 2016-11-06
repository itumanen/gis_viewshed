#include "grid.h" 

#define VIEW_NOT_COMPUTED -1

class View_Grid : public Grid {
public:
	// Constructor
	View_Grid(Grid* elevGrid, int vp_row, int vp_col);

	// Methods
	void initialize();
	void computeViewshed(Grid* elevGrid);
	int isVisible(Grid* elevGrid, int row, int col);

private:
	int vp_row;
	int vp_col;

};

