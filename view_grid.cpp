#include "view_grid.h"
#include <math.h>

bool VERTICAL = true; 

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

// COMPUTE IN QUADRANTS to help with debugging
// Isolate horizontal/vertical lines

// Iterates through elevation grid and calls isVisible
// Sets grid value to 0 or 1 if point is visible from given
// viewpoint
void View_Grid::computeViewshed() {
	for (int i = this->getVProw(); i < this->numRows; i++) {
		for (int j = this->getVPcol(); j < this->numCols; j++) {
			this->setGridValueAt(i, j, isVisible(i, j));
		}
	}
}

// Interpolates between point at given row/col coordinates
// and viewpoint coordinates. Returns 0 if the point is not
// visible and 1 if the point is visible 
int View_Grid::isVisible(int row, int col) {

	// interpolate to determine LOS between VP and (row, col)
	float LOS = getVerticalAngle(row, col); // vertical angle of the line of sight
	float slope = getSlope(row, col); // slope of the line between VP and P
	for (int i = this->getVProw(); i < row; i++) {
		for (int j = this->getVPcol(); j < col; j++) {
			// determine intersection between LOS and vertical axis
			float r = getIntersectionRow(slope, j);
		}
	}
	// interpolate to determine height of points p on vert/horiz axes
	// compare heights at point p
	return NOT_VISIBLE;
}

float View_Grid::getHeight(int iteration, int row, int col) {
	// return interpolate * distance
	return this->getVerticalAngle(row, col) * iteration;
}

// compute tan x, where x is the angle formed by viewpoint elevation
// and elevation of nearest point in path to (row, col)
// tan(x) = (ha - hv) / d(a,v) where v is vp and a is point
float View_Grid::getVerticalAngle(float row, float col) {
	return (elevGrid->getGridValueAt(row, col) - elevGrid->getGridValueAt(this->getVProw(), this->getVPcol())) /
		sqrt(pow(row - this->getVProw(),2) + pow(col - this->getVPcol(), 2));
}

// Returns second coordinate of intersection point on a vertical axis
float View_Grid::getIntersectionRow(float slope, int col) {
	return slope * col;
}

// Considers VP as the "origin" of a coordinate system in order to consider
// only the slope of the line equation y = mx, where y is the lines through
// the view point and the point of consideration p
// Input: coordinates of the point p
float View_Grid::getSlope(int row, int col) {
	return 0.1;
	// float slope = (row - getVProw()) / (col - getVPcol());
	// cout << "test" << endl; return slope;
	return static_cast<float> (row - getVProw()) / (col - getVPcol());
}