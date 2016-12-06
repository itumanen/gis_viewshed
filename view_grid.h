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
	void lowerRightQuadAxes();
	void lowerLeftQuadAxes();
	void upperLeftQuadAxes();
	void upperRightQuadAxes();
	float getVerticalAngle(int row, int col, float value);
	float getSlope(int row, int col);
	float getYIntercept(float slope);
	float interpolate(float index, int row, int col);
	float getIntersection(float slope, float yIntercept, int row, int col);


private:
	Grid* elevGrid;
	int vp_row;
	int vp_col;
};

