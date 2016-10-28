#include "fd_grid.h"

/* FLOW DIRECTION GRID METHODS */

FD_Grid::FD_Grid(Grid* elevGrid) {

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

}

int FD_Grid::getDirectionCode(int i, int j) {
	
	if (i == -1) {
		if (j == -1) {
			return NORTHWEST;
		}
		if (j == 0) {
			return NORTH;
		}
		if (j == 1) {
			return NORTHEAST;
		}
	}

	if (i == 0) {
		if (j == -1) {
			return WEST;
		}
		if (j == 1) {
			return EAST;
		}
	}

	if (i == 1) {
		if (j == -1) {
			return SOUTHWEST;
		}
		if (j == 0) {
			return SOUTH;
		}
		if (j == 1) {
			return SOUTHEAST;
		}
	}

	return NO_FLOW;
}

int FD_Grid::computeFlowDirAt(Grid* elevGrid, int i, int j) {

	float minElevation = elevGrid->getGridValueAt(i, j);
	int verticalDirection = 0;
	int horizontalDirection = 0;

	for (int row = -1; row < 2; row++) {

		// boundary  check
		if (row + i < 0 || row + i > this->numRows - 1) {
			continue;
		}
		
		for (int col = -1; col < 2; col++) {
			
			// boundary check
			if (col + j < 0 || col + j > this->numCols - 1) {
				continue;
			}
			else if (row + i == 0 && col + j == 0) {
				continue;
			}
			// nodata value check

			else if (elevGrid->getGridValueAt(i, j) == elevGrid->getNodataValue()) {
				continue;
			}

			if (elevGrid->getGridValueAt(i + row, j + col) < minElevation) {
				minElevation = elevGrid->getGridValueAt(i + row, j + col);
				verticalDirection = row;
				horizontalDirection = col;
			}

		}

	}
	
	if (minElevation == elevGrid->getGridValueAt(i, j)) {
		return NO_FLOW;
	}
	
	return this->getDirectionCode(verticalDirection, horizontalDirection);

}

void FD_Grid::computeFlowDir(Grid* elevGrid) {
	for (int row = 0; row < this->numRows; row++) {
		for (int col = 0; col < this->numCols; col++) {
			this->setGridValueAt(row,col, computeFlowDirAt(elevGrid, row, col));
		}
	}
}