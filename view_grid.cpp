#include "view_grid.h"
#include <math.h>

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
void View_Grid::computeViewshed() {
	for (int i = 0; i < this->numRows; i++) {
		for (int j = 0; j < this->numCols; j++) {
			this->setGridValueAt(i, j, isVisible(i, j));
		}
	}

}

// COMPUTE IN QUADRANTS to help with debugging
// Isolate horizontal/vertical lines

// Interpolates between point at given row/col coordinates
// and viewpoint coordinates. Returns 0 if the point is not
// visible and 1 if the point is visible 
int View_Grid::isVisible(int row, int col) {
	return 0;
}

// compute tan x, where x is the angle formed by viewpoint elevation
// and elevation of nearest point in path to (row, col)
// tan(x) = (ha - hv) / d(a,v) where v is vp and a is point
float View_Grid::interpolate(int row, int col) {

	return (elevGrid->getGridValueAt(row, col) - elevGrid->getGridValueAt(this->getVProw(), this->getVPcol())) /
		sqrt(pow(row - this->getVProw(),2) + pow(col - this->getVPcol(), 2));

}