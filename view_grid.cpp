#include "view_grid.h"
#include <math.h>

bool HORIZONTAL = true; 
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
	for (int i = this->getVProw(); i < this->getNumRows(); i++) {
		for (int j = this->getVPcol(); j < this->getNumCols(); j++) {
			if (elevGrid->getGridValueAt(i, j) == getNodataValue()) this->setGridValueAt(i, j, getNodataValue());
			this->setGridValueAt(i, j, isVisible(i, j));
		}
	}

}

int View_Grid::compareHeights(int row, int col) {
	return (elevGrid->getGridValueAt(getVProw(), getVPcol()) > elevGrid->getGridValueAt(row,col));
}

// Interpolates between point at given row/col coordinates
// and viewpoint coordinates. Returns 0 if the point is not
// visible and 1 if the point is visible 
int View_Grid::isVisible(int row, int col) {

	float LOS = getVerticalAngle(col, this->elevGrid->getGridValueAt(row,col)); // vertical angle of the line of sight between VP and P(row,col)
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
	for (int i = this->getVPcol() + 1; i < col + 1; i++) {
		float interR = getIntersectionRow(slope, yIntercept, i);
		float height = interpolate(interR, i);
		if (getVerticalAngle(i, height) > LOS) return NOT_VISIBLE;

	}

	// TODO HORIZONTAL INTERSECTIONS

	if (DEBUG) cout << "visible" << endl;
	return VISIBLE;
}

float View_Grid::getHeight(float intersect, float verticalAngle) {
	return intersect / tan(verticalAngle); // TODO check this
}

// compute tan x, where x is the angle formed by viewpoint elevation
// and elevation of nearest point in path to (row, col)
// tan(x) = (ha - hv) / d(a,v) where v is vp and a is point
// TODO absolute values to work with multiple quadrants
float View_Grid::getVerticalAngle(float col, float value) {
	return atan((value - elevGrid->getGridValueAt(this->getVProw(), this->getVPcol()))
				/ (col - getVPcol()));
}

// height at intersection = d * tan(a)
float View_Grid::interpolate(float interR, int col) {
	return ((this->elevGrid->getGridValueAt(ceil(interR), col) - this->elevGrid->getGridValueAt(floor(interR), col))
			* (interR - floor(interR)))
			+ this->elevGrid->getGridValueAt(floor(interR), col);
}


// Returns second coordinate of intersection point on a vertical axis
float View_Grid::getIntersectionRow(float slope, float yIntercept, int row, int col) {
	if(DEBUG && slope*col+yIntercept < this->getNumCols()) printf("intersection row is %f at %d\n", slope * col + yIntercept, col); fflush(stdout);
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