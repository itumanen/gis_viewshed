#include "grid.h" 

#define VIEW_NOT_COMPUTED -1
#define NOT_VISIBLE 0
#define IS_VISIBLE 1

class View_Grid : public Grid {
public:
	// Constructor
	View_Grid(Grid* elevGrid, int vp_row, int vp_col);

	// Getters
	int getVProw() { return this->vp_row; }
	int getVPcol() { return this->vp_col; }

	// Setters
	void setVProw(int vp_row) { this->vp_row = vp_row; }
	void setVPcol(int vp_col) { this->vp_col = vp_col; }

	// Methods
	void initialize();
	void computeViewshed(Grid* elevGrid);
	int isVisible(Grid* elevGrid, int row, int col);

private:
	int vp_row;
	int vp_col;

};

