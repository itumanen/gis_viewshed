#include "grid.h" 

#define VIEW_NOT_COMPUTED -1
#define NOT_VISIBLE 0
#define VISIBLE 1


/* VIEW GRID CLASS
	Inherited from Grid class; implements methods to determine
	whether other points in the grid are visible from a given
	view point.
 */
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
	void computeViewshed();
	int isVisible(int row, int col);
	int compareHeights(int row, int col);
	float getVerticalAngle(float col, float value);
	float getSlope(int row, int col);
	float getYIntercept(float slope);
	float interpolate(float interR, int col);
	float getHeight(float intersect, float VerticalAngle);
	float getIntersectionRow(float slope, float yIntercept, int col);


private:
	Grid* elevGrid;
	int vp_row;
	int vp_col;
};



// determine height at (intersect, col) by interpolation
	// find vertical angle between rows.
// compute vertical angle from viewpoint to (intersect, col)
// if that angle smaller than vertical angle to (row, col): point not visible, return NOT_VISIBLE

	// for (int c = this->getVPcol(); c < this->numCols; c++) {
	// 	float r = getIntersectionRow(slope, yIntercept, c);
	// 	float tanX = (elevGrid->getGridValueAt(row, col) - elevGrid->getGridValueAt(row + 1, col))
	// 				/ (col - getVPcol());
	// 	float height = r / tanX;
	// 	float verticalAngle = (r - getVProw() / height - elevGrid->getGridValueAt(getVProw(),getVPcol()));
	// 	if (DEBUG) printf("verticalAngle is %f\n", verticalAngle); fflush(stdout);
	// 	// compare vertical angles
	// 	// return NOT_VISIBLE;
	// }

