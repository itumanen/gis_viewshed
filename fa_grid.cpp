#include "fa_grid.h"

/* FLOW ACCUMULATION GRID METHODS */

FA_Grid::FA_Grid(Grid* elevGrid) {

	this->numCols = elevGrid->getNumCols();
	this->numRows = elevGrid->getNumRows();
	this->xllCorner = elevGrid->getXllCorner();
	this->yllCorner = elevGrid->getYllCorner();

	this->cellSize = elevGrid->getCellSize();

	this->minVal = elevGrid->getMinVal();
	this->maxVal = elevGrid->getMaxVal();

	this->nodata_value = elevGrid->getNodataValue();

	// allocate array
	this->gridVals = (float**) malloc(sizeof(float*) * this->numRows);
	assert(this->gridVals);
	for (int i = 0; i < this->numRows; i++) {
		this->gridVals[i] = (float*) malloc(sizeof(float) * this->numCols);
		assert(this->gridVals[i]);
	}

	this->initializeGrid(elevGrid);

	// allocate boolean array
	this->computedGrid = (bool**) malloc(sizeof(bool*) * this->numRows);
	assert(this->computedGrid);
	for (int i = 0; i < this->numRows; i++) {
		this->computedGrid[i] = (bool*) malloc(sizeof(bool) * this->numCols);
		assert(this->computedGrid[i]);
	}

	this->initializeEmptyGrids();

}

void FA_Grid::initializeGrid(Grid* elevGrid) {

	for (int i = 0; i < this->numRows; i++) {
		for (int j = 0; j < this->numCols; j++) {

			if (this->getGridValueAt(i,j) != this->nodata_value) {
				this->setGridValueAt(i, j, INITIAL_VALUE);
				continue;
			}
			
			this->setGridValueAt(i, j, this->nodata_value);
		}
	}
}

void FA_Grid::initializeEmptyGrids() {

	for (int i = 0; i < this->numRows; i++) {
		for (int j = 0; j < this->numCols; j++) {
			this->computedGrid[i][j] = 0;
		}
	}

}

float FA_Grid::computeFlowAt(Grid* fdGrid, int i, int j) {

	if (computedGrid[i][j]) {
		return this->getGridValueAt(i,j);
	}

	if (this->isNoData(i, j)) {
		// return zero - don't change flow accumulation
		return 0;
	}

	for (int row = -1; row < 2; row++) {

		if (row + i < 0 || row + i > this->numRows - 1) {
			continue;
		}
		
		for (int col = -1; col < 2; col++) {

			if (col + j < 0 || row + j > this->numCols - 1) {
				continue;
			}
			
			if (this->flowsInto(fdGrid, i, j, row, col)) {  // recurse on this cell to compute total flow
				this->setGridValueAt(i, j, this->getGridValueAt(i, j) + this->computeFlowAt(fdGrid, i + row, j + col));
			}

		}

	}

	// return accumulated flow value; returns 1 if nothing flows in
	this->computedGrid[i][j] = true;
	return this->getGridValueAt(i, j);

}

void FA_Grid::computeFlow(Grid* fdGrid) {
	for (int row = 0; row < this->numRows; row++) {
		for (int col = 0; col < this->numCols; col++) {
			if (this->isNoData(row, col)) {
				// never calls on a nodata cell from here
				continue;
			}
			this->setGridValueAt(row, col, this->computeFlowAt(fdGrid, row, col));	
		}
	}
}

bool FA_Grid::flowsInto(Grid* fdGrid, int i, int j, int offsetX, int offsetY) {

	if (fdGrid->getGridValueAt(i + offsetX, j + offsetY) == WEST && offsetX == 0 && offsetY == 1) {
		return true;
	}
	if (fdGrid->getGridValueAt(i + offsetX, j + offsetY) == NORTHWEST && offsetX == 1 && offsetY == 1) {
		return true;
	}
	if (fdGrid->getGridValueAt(i + offsetX, j + offsetY) == NORTH && offsetX == 1 && offsetY == 0) {
		return true;
	}
	if (fdGrid->getGridValueAt(i + offsetX, j + offsetY) == NORTHEAST && offsetX == 1 && offsetY == -1) {
		return true;
	}
	if (fdGrid->getGridValueAt(i + offsetX, j + offsetY) == EAST && offsetX == 0 && offsetY == -1) {
		return true;
	}
	if (fdGrid->getGridValueAt(i + offsetX, j + offsetY) == SOUTHEAST && offsetX == -1 && offsetY  == -1) {
		return true;
	}
	if (fdGrid->getGridValueAt(i + offsetX, j + offsetY) == SOUTH && offsetX == -1 && offsetY  == 0) {
		return true;
	}
	if (fdGrid->getGridValueAt(i + offsetX, j + offsetY) == SOUTHWEST && offsetX == -1 && offsetY  == 1) {
		return true;
	}
	
	return NO_FLOW;
}