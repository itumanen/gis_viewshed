#include "view_grid.h"
#include <math.h>

bool HORIZONTAL = false; 
bool DEBUG = false;
bool DEBUG_I = false;

// CONSTRUCTOR 
// Reads in values from elevation grid
// Allocates 2D grid array and calls initialization method
View_Grid::View_Grid(Grid* elevGrid, int vp_row, int vp_col) {

	// Set viewpoint coordinates
	this->setVProw(vp_row);
	this->setVPcol(vp_col);

	// Set grid attributes
	this->setNumCols(elevGrid->getNumCols());
	this->setNumRows(elevGrid->getNumRows());
	this->setXllCorner(elevGrid->getXllCorner());
	this->setYllCorner(elevGrid->getYllCorner());
	this->setCellSize(elevGrid->getCellSize());
	this->setMinVal(elevGrid->getMinVal());
	this->setMaxVal(elevGrid->getMaxVal());
	this->setNodataValue(elevGrid->getNodataValue());
	
	this->elevGrid = elevGrid;

	// Allocate array
	this->gridVals = (float**) malloc(sizeof(float*) * this->numRows);
	assert(this->gridVals);
	for (int i = 0; i < this->numRows; i++) {
		this->gridVals[i] = (float*) malloc(sizeof(float) * this->numCols);
		assert(this->gridVals[i]);
	}

	// Set all values to -1
	this->initialize();

}

// Called by View_Grid constructor
// Iterates through all cells in the newly allocated 2D grid
// and sets the values to -1
void View_Grid::initialize() {
	for (int row = 0; row < this->numRows; row++) {
		for (int col = 0; col < this->numCols; col++) {
			this->setGridValueAt(row, col, VIEW_NOT_COMPUTED);
		}
	}
}

// Iterates through elevation grid and calls isVisible
// Sets grid value to 0 or 1 if point is visible from given
// viewpoint
// TODO three other quadrants
// TODO look at horizontal intersections too 
void View_Grid::computeViewshed() {

	// LOWER RIGHT QUADRANT
	lowerRightQuadAxes();
	for (int i = this->getVProw() + 1; i < this->getNumRows(); i++) {
		for (int j = this->getVPcol() + 1; j < this->getNumCols(); j++) {
			if (elevGrid->getGridValueAt(i, j) == getNodataValue()) {
				this->setGridValueAt(i, j, getNodataValue());	
			} 
			this->setGridValueAt(i, j, isVisible(i, j));
		}
	}

}

void View_Grid::lowerRightQuadAxes() {
	// set neighbors on VP row and col to visible
	this->setGridValueAt(vp_row, vp_col + 1, VISIBLE);
	this->setGridValueAt(vp_row + 1, vp_col, VISIBLE);

	// keep track of maximum vertical angle
	float LOS = getVerticalAngle(vp_row, vp_col + 1, this->elevGrid->getGridValueAt(vp_row, vp_col + 1));
	for (int col = this->vp_col + 2; col < this->getNumCols(); col++) {
		float verticalAngle = getVerticalAngle(vp_row, col, this->elevGrid->getGridValueAt(vp_row, col));
		if(verticalAngle > LOS) {
			this->setGridValueAt(vp_row, col, VISIBLE);
			LOS = verticalAngle;
			continue;
		}
		this->setGridValueAt(vp_row, col, NOT_VISIBLE);
	} 

	HORIZONTAL = true;
	LOS = getVerticalAngle(getVProw() + 1, getVPcol(), this->elevGrid->getGridValueAt(getVProw() + 1, getVPcol()));

	for (int row = this->getVProw() + 2; row < this->getNumRows(); row++) {
		float verticalAngle = getVerticalAngle(row, vp_col, this->elevGrid->getGridValueAt(row, vp_col));
		if(verticalAngle > LOS) {
			this->setGridValueAt(row, vp_col, VISIBLE);
			LOS = verticalAngle;
			continue;
		}
		this->setGridValueAt(row, getVPcol(), NOT_VISIBLE);
	} 

	HORIZONTAL = false;
}

// Interpolates between point at given row/col coordinates
// and viewpoint coordinates. Returns 0 if the point is not
// visible and 1 if the point is visible 
int View_Grid::isVisible(int row, int col) {

	float LOS = getVerticalAngle(row, col, this->elevGrid->getGridValueAt(row,col)); // vertical angle of the line of sight between VP and P(row,col)
	float slope = getSlope(row, col); // slope of the line between VP and P
	float yIntercept = getYIntercept(slope); // line equation: col = row * slope + yIntercept

	if (DEBUG) { 
		printf("\nslope is %f at %d, %d\n", slope, row, col); fflush(stdout);
		printf("yIntercept is %f at %d, %d\n", yIntercept, row, col); fflush(stdout);
	}

	// go through vertical lines between VP and P
	// find intersection between vertical and LOS
	// interpolate between rows to compute height at intersection
	// compute vertical angle between VP and intersection

	for (int j = this->getVPcol() + 1; j < col + 1; j++) {
		float interR = getIntersection(slope, yIntercept, row, j);
		float height = interpolate(interR, row, j);
		if (getVerticalAngle(row, j, height) > LOS) return NOT_VISIBLE;
	}

	// check with horizontal intersections
	HORIZONTAL = true;
	for (int i = vp_row + 1; i < row + 1; i++) {
		float interC = getIntersection(slope, yIntercept, i, col); 
		float height = interpolate(interC, i, col);
		if(getVerticalAngle(i, col, height) > LOS) return NOT_VISIBLE;
	}
	HORIZONTAL = false;

	if (DEBUG) cout << "visible" << endl;
	return VISIBLE;
}

// compute tan x, where x is the angle formed by viewpoint elevation
// and elevation of nearest point in path to (row, col)
// tan(x) = (ha - hv) / d(a,v) where v is vp and a is point
// TODO absolute values to work with multiple quadrants
// input value is elevation at comparison point
float View_Grid::getVerticalAngle(int row, int col, float value) {
	float distance = sqrt( pow(row - vp_row, 2) + pow(col - vp_col, 2) );
	if (HORIZONTAL) {
		return atan((value - elevGrid->getGridValueAt(vp_row, vp_col)) 
					/ distance);
	}
	return atan((value - elevGrid->getGridValueAt(this->getVProw(), this->getVPcol()))
				/ distance);
}

// height at intersection = d * tan(a)
float View_Grid::interpolate(float index, int row, int col) {
	if (HORIZONTAL) {
		return ((this->elevGrid->getGridValueAt(row, ceil(index)) - this->elevGrid->getGridValueAt(row, floor(index)))
			* (index - floor(index)))
			+ this->elevGrid->getGridValueAt(row, floor(index));
	}
	return ((this->elevGrid->getGridValueAt(ceil(index), col) - this->elevGrid->getGridValueAt(floor(index), col))
			* (index - floor(index)))
			+ this->elevGrid->getGridValueAt(floor(index), col);
}


// Returns second coordinate of intersection point on a vertical axis
float View_Grid::getIntersection(float slope, float yIntercept, int row, int col) {
	if(HORIZONTAL) return (row - yIntercept) / slope;
	return slope * col + yIntercept;
}

// Computes slope of equation of line between VP and given point p
// Input: coordinates of the point p
float View_Grid::getSlope(int row, int col) {
	return static_cast<float> (row - getVProw()) / (col - getVPcol());
}

// Computes y-intercept of equation of line through VP and p using the slope
float View_Grid::getYIntercept(float slope) {
	return getVPcol() - getVProw() * slope;
}