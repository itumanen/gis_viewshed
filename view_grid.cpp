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

void View_Grid::computeViewshed() {

	// LOWER RIGHT QUADRANT
	if (vp_row < numRows - 1 && vp_col < numCols - 1) {
		lowerRightQuadAxes();
		lowerRightQuad();
	}

	// // LOWER LEFT QUADRANT
	if (vp_row < numRows - 1 && vp_col > 0) {
		lowerLeftQuadAxes();
		lowerLeftQuad();
	}

	// UPPER LEFT QUADRANT
	if(vp_row > 0 && vp_col > 0) {
		upperLeftQuadAxes();
		upperLeftQuad();
	}

	// // UPPER RIGHT QUADRANT
	if (vp_row > 0 && vp_col < numCols - 1) {
		upperRightQuadAxes();
		upperRightQuad();
	}

	setGridValueAt(vp_row, vp_col, VISIBLE);

}

void View_Grid::lowerRightQuad() {
	for (int i = this->getVProw() + 1; i < this->getNumRows(); i++) {
		for (int j = this->getVPcol() + 1; j < this->getNumCols(); j++) {
			if (i == vp_row + 1 && j == vp_col + 1) {
				setGridValueAt(i,j,VISIBLE); 
				continue; 
			}
			if (elevGrid->getGridValueAt(i, j) == getNodataValue()) {
				this->setGridValueAt(i, j, getNodataValue());
				continue;	
			} 
			this->setGridValueAt(i, j, isVisible(i, j, LOWER_RIGHT));
		}
	}
}

void View_Grid::lowerLeftQuad() {
	for (int i = this->getVProw() + 1; i < this->getNumRows(); i++) {
		for (int j = this->getVPcol() - 1; j > -1; j--) {
			if (i == vp_row + 1 && j == vp_col - 1) { // set diagonal neighbor to visible
				setGridValueAt(i,j,VISIBLE); 
				continue; 
			}
			if (elevGrid->getGridValueAt(i, j) == getNodataValue()) {
				this->setGridValueAt(i, j, getNodataValue());
				continue;	
			} 
			this->setGridValueAt(i, j, isVisible(i, j, LOWER_LEFT));
		}
	}
}

void View_Grid::upperLeftQuad() {
	for (int i = this->getVProw() - 1; i > -1; i--) {
		for (int j = this->getVPcol() - 1; j > -1; j--) {
			if (i == vp_row - 1 && j == vp_col - 1) { // set diagonal neighbor to visible
				setGridValueAt(i,j,VISIBLE); 
				continue; 
			}
			if (elevGrid->getGridValueAt(i, j) == getNodataValue()) {
				this->setGridValueAt(i, j, getNodataValue());
				continue;	
			} 
			this->setGridValueAt(i, j, isVisible(i, j, UPPER_LEFT));
		}
	}
}

void View_Grid::upperRightQuad() {
	for (int i = this->getVProw() - 1; i > -1; i--) {
		for (int j = this->getVPcol() + 1; j < this->numCols; j++) {
			if (i == vp_row - 1 && j == vp_col + 1) { // set diagonal neighbor to visible
				setGridValueAt(i,j,VISIBLE); 
				continue; 
			}
			if (elevGrid->getGridValueAt(i, j) == getNodataValue()) {
				this->setGridValueAt(i, j, getNodataValue());
				continue;	
			} 
			this->setGridValueAt(i, j, isVisible(i, j, UPPER_RIGHT));
		}
	}
}

void View_Grid::lowerRightQuadAxes() {

	// set neighbors on VP row and col to visible
	setGridValueAt(vp_row, vp_col + 1, VISIBLE);
	setGridValueAt(vp_row + 1, vp_col, VISIBLE);

	HORIZONTAL = true;
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
	HORIZONTAL = false;

	// float LOS = getVerticalAngle(getVProw() + 1, getVPcol(), this->elevGrid->getGridValueAt(getVProw() + 1, getVPcol()));
	for (int row = this->getVProw() + 2; row < this->getNumRows(); row++) {
		float verticalAngle = getVerticalAngle(row, vp_col, this->elevGrid->getGridValueAt(row, vp_col));
		if(verticalAngle > LOS) {
			this->setGridValueAt(row, vp_col, VISIBLE);
			LOS = verticalAngle;
			continue;
		}
		this->setGridValueAt(row, getVPcol(), NOT_VISIBLE);
	} 

}

void View_Grid::lowerLeftQuadAxes() {

	// redundancy check on vertical axis -- if no lower right quadrant
	bool redundant = false;
	if (getGridValueAt(vp_row + 1, vp_col) != VIEW_NOT_COMPUTED) redundant = true;

	// set neighbors on VP row and col to visible
	setGridValueAt(vp_row, vp_col - 1, VISIBLE);
	setGridValueAt(vp_row + 1, vp_col, VISIBLE);

	HORIZONTAL = true;
	float LOS = getVerticalAngle(vp_row, vp_col - 1, this->elevGrid->getGridValueAt(vp_row, vp_col - 1));
	for (int col = vp_col - 2; col > -1; col--) {
		float verticalAngle = getVerticalAngle(vp_row, col, this->elevGrid->getGridValueAt(vp_row, col));
		if(verticalAngle > LOS) {
			setGridValueAt(vp_row, col, VISIBLE);
			LOS = verticalAngle; // keep track of max vertical angle
			continue;
		}
		setGridValueAt(vp_row, col, NOT_VISIBLE);
	} 
	HORIZONTAL = false;

	if (!redundant) {

		float LOS = getVerticalAngle(getVProw() + 1, getVPcol(), this->elevGrid->getGridValueAt(getVProw() + 1, getVPcol()));
		for (int row = this->getVProw() + 2; row < this->getNumRows(); row++) {
			float verticalAngle = getVerticalAngle(row, vp_col, this->elevGrid->getGridValueAt(row, vp_col));
			if(verticalAngle > LOS) {
				this->setGridValueAt(row, vp_col, VISIBLE);
				LOS = verticalAngle;
				continue;
			}
			setGridValueAt(row, getVPcol(), NOT_VISIBLE);
		} 

	}

}

void View_Grid::upperLeftQuadAxes() {

	// redundancy check on vertical axis
	bool redundant = false;
	if (getGridValueAt(vp_row - 1, vp_col) != VIEW_NOT_COMPUTED) redundant = true;

	// set neighbors on VP row and col to visible
	setGridValueAt(vp_row, vp_col - 1, VISIBLE);
	setGridValueAt(vp_row - 1, vp_col, VISIBLE);

	float LOS = getVerticalAngle(vp_row - 1, vp_col, this->elevGrid->getGridValueAt(vp_row - 1, vp_col));
	for (int row = this->getVProw() - 2; row > -1; row--) {
		float verticalAngle = getVerticalAngle(row, vp_col, this->elevGrid->getGridValueAt(row, vp_col));
		if(verticalAngle > LOS) {
			this->setGridValueAt(row, vp_col, VISIBLE);
			LOS = verticalAngle;
			continue;
		}
		this->setGridValueAt(row, getVPcol(), NOT_VISIBLE);
	} 

	if (!redundant) {

		HORIZONTAL = true;
		float LOS = getVerticalAngle(vp_row, vp_col - 1, this->elevGrid->getGridValueAt(vp_row, vp_col - 1));
		for (int col = vp_col - 2; col > -1; col--) {
			float verticalAngle = getVerticalAngle(vp_row, col, this->elevGrid->getGridValueAt(vp_row, col));
			if(verticalAngle > LOS) {
				this->setGridValueAt(vp_row, col, VISIBLE);
				LOS = verticalAngle;
				continue;
			}
			this->setGridValueAt(vp_row, col, NOT_VISIBLE);
		} 
		HORIZONTAL = false;

	}

}

void View_Grid::upperRightQuadAxes() {

	bool redundantC = false;
	bool redundantR = false;

	if (getGridValueAt(vp_row - 1, vp_col) != VIEW_NOT_COMPUTED) redundantR = true;
	if (getGridValueAt(vp_row, vp_col + 1) != VIEW_NOT_COMPUTED) redundantC = true;
	if (redundantC && redundantR) return; // axes already computed

	// set neighbors on VP row and col to visible
	setGridValueAt(vp_row, vp_col + 1, VISIBLE);
	setGridValueAt(vp_row + 1, vp_col, VISIBLE);

	float LOS = getVerticalAngle(vp_row, vp_col + 1, this->elevGrid->getGridValueAt(vp_row, vp_col + 1));
	if (!redundantC) {
		HORIZONTAL = true;
		for (int col = this->vp_col + 2; col < this->getNumCols(); col++) {
			float verticalAngle = getVerticalAngle(vp_row, col, this->elevGrid->getGridValueAt(vp_row, col));
			if(verticalAngle > LOS) {
				this->setGridValueAt(vp_row, col, VISIBLE);
				LOS = verticalAngle;
				continue;
			}
			this->setGridValueAt(vp_row, col, NOT_VISIBLE);
		} 
		HORIZONTAL = false;
	}

	LOS = getVerticalAngle(getVProw() - 1, getVPcol(), this->elevGrid->getGridValueAt(getVProw() - 1, getVPcol()));
	if (!redundantR) {
		for (int row = this->getVProw() - 2; row > -1; row--) {
			float verticalAngle = getVerticalAngle(row, vp_col, this->elevGrid->getGridValueAt(row, vp_col));
			if(verticalAngle > LOS) {
				this->setGridValueAt(row, vp_col, VISIBLE);
				LOS = verticalAngle;
				continue;
			}
			this->setGridValueAt(row, getVPcol(), NOT_VISIBLE);
		} 
	}

}
// Interpolates between point at given row/col coordinates
// and viewpoint coordinates. Returns 0 if the point is not
// visible and 1 if the point is visible 
int View_Grid::isVisible(int row, int col, int quadrant) {
	
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

	if (quadrant == LOWER_RIGHT) {
		for (int j = this->getVPcol() + 1; j < col; j++) {
			float interR = getIntersection(slope, yIntercept, row, j);
			float height = checkNodataInterpolate(interR, col);
			if (height == nodata_value) {
				continue;
			}
			else if (height == nodata_value - 1) {
				height = interpolate(interR, row, j);
			}
			float verticalAngle = getVerticalAngle(interR, j, height);
			if (verticalAngle > LOS) return NOT_VISIBLE;
		}

		// check with horizontal intersections
		HORIZONTAL = true;
		for (int i = vp_row + 1; i < row; i++) {
			float interC = getIntersection(slope, yIntercept, i, col);
			float height = checkNodataInterpolate(row, interC);
			if (height == nodata_value) {
				continue;
			}
			else if (height == nodata_value - 1) {
				height = interpolate(interC, i, col);
			}
			float verticalAngle = getVerticalAngle(row, interC, height);
			if (verticalAngle > LOS) return NOT_VISIBLE;
		}
		HORIZONTAL = false;
		return VISIBLE;
	}




	if (quadrant == LOWER_LEFT) {
		for (int j = this->getVPcol() - 1; j > col; j--) {
			float interR = getIntersection(slope, yIntercept, row, j);
			float height = checkNodataInterpolate(interR, col);
			if(height == nodata_value) {
				continue;
			}
			else if (height == nodata_value - 1) {
				height = interpolate(interR, row, j);
			}
			if (getVerticalAngle(interR, j, height) > LOS) return NOT_VISIBLE;
		}

		// check with horizontal intersections
		HORIZONTAL = true;
		for (int i = vp_row + 1; i < row; i++) {
			float interC = getIntersection(slope, yIntercept, i, col); 
			float height = checkNodataInterpolate(row, interC);
			if (height == nodata_value) {
				continue;
			}
			else if (height == nodata_value - 1) {
				height = interpolate(interC, i, col);
			}
			float verticalAngle = getVerticalAngle(i, interC, height);
			if(verticalAngle > LOS) return NOT_VISIBLE;
		}
		HORIZONTAL = false;

		if (DEBUG) cout << "visible" << endl;
		return VISIBLE;
	}

	if (quadrant == UPPER_LEFT) {
		for (int j = this->vp_col - 1; j > col; j--) {

			float interR = getIntersection(slope, yIntercept, row, j);
			float height = checkNodataInterpolate(interR, col);
			if (height == nodata_value) {
				continue;
			}
			else if (height == nodata_value - 1) {
				height = interpolate(interR, row, j);
				if (row == 0 && col == 0) {
					cout << "updated height by interpolating " << endl;
				}
			}

			float verticalAngle = getVerticalAngle(interR, j, height);
			if (row == 0 && col == 0) {
				cout << "interR is " << interR << " height is " << height << endl;
				cout << "LOS IS " << LOS << " and vertical angle is " << verticalAngle << endl;
			}



			if (getVerticalAngle(interR, j, height) > LOS) { 
				if (row == 0 && col == 0) {
					cout << "returning not visible" << endl; 
					return NOT_VISIBLE;
				}
			}
		}
		

		// check with horizontal intersections
		HORIZONTAL = true;
		if (row == 0 && col == 0) cout << "HORIZONTAL IS TRUE " << endl;
		for (int i = vp_row - 1; i > row; i--) {
			float interC = getIntersection(slope, yIntercept, i, col); 
			float height = checkNodataInterpolate(row, interC);
			if (height == nodata_value) {
				continue;
			}
			else if (height == nodata_value - 1) {
				height = interpolate(interC, i, col);
				if (row == 0 && col == 0) {
					cout << "updating height by interpolating " << endl;
				}
			}
			float verticalAngle = getVerticalAngle(i, interC, height);

			if (row == 0 && col == 0) {
				cout << "interC is " << interC << " height is " << height << endl;
				cout << "LOS IS " << LOS << " and vertical angle is " << verticalAngle << endl;
			}
			if(getVerticalAngle(i, interC, height) > LOS) {
				if (row == 0 && col == 0) cout << "returning not visible" << endl; 
				return NOT_VISIBLE;
			}
		}
		HORIZONTAL = false;
		if (row == 0 && col == 0) cout << "SWITCHED HORIZONTAL TO FALSE" << endl;

		if (DEBUG) cout << "visible" << endl;
		return VISIBLE;
	}

	if (quadrant == UPPER_RIGHT) {
		for (int j = this->getVPcol() + 1; j < col; j++) {
			float interR = getIntersection(slope, yIntercept, row, j);
			float height = checkNodataInterpolate(interR, col);
			if (height == nodata_value) {
				continue;
			}
			else if (height == nodata_value - 1) {
				height = interpolate(interR, row, j);
			}
			if (getVerticalAngle(interR, j, height) > LOS) return NOT_VISIBLE;
		}

		// check with horizontal intersections
		HORIZONTAL = true;
		for (int i = vp_row - 1; i > row; i--) {
			float interC = getIntersection(slope, yIntercept, i, col); 
			float height = checkNodataInterpolate(row, interC);
			if (height == nodata_value) {
				continue;
			}
			else if (height == nodata_value - 1) {
				height = interpolate(interC, i, col);
			}
			if(getVerticalAngle(i, interC, height) > LOS) return NOT_VISIBLE;
		}
		HORIZONTAL = false;
		return VISIBLE;
	}

	return VISIBLE;
}

float View_Grid::checkNodataInterpolate(float row, float col) {
	int count = 0;
	float height = 0;

	if (HORIZONTAL) {
		if (elevGrid->getGridValueAt(row, floor(col)) == nodata_value) {
			count++;
		}
		else {
			height += getGridValueAt(row, floor(col));
		}
		if (elevGrid->getGridValueAt(row, ceil(col)) == nodata_value) {
			count++;
		}
		else {
			height += getGridValueAt(row, floor(col));
		}
	}

	else {
		if (elevGrid->getGridValueAt(floor(row), col) == nodata_value) {
			count++;
		}
		else {
			height += elevGrid->getGridValueAt(floor(row), col);
		}
		if(elevGrid->getGridValueAt(ceil(row), col) == nodata_value) {
			count++;
		}
		else {
			height += elevGrid->getGridValueAt(floor(row), col);
		}
	}

	if (count == 0) {
		return nodata_value - 1;
	}
	else if (count == 1) {
		return height;
	} else if (count == 2) {
		return nodata_value;
	}

	return nodata_value;
}

// compute tan x, where x is the angle formed by viewpoint elevation
// and elevation of nearest point in path to (row, col)
// tan(x) = (ha - hv) / d(a,v) where v is vp and a is point
// TODO absolute values to work with multiple quadrants // TODO NEED THIS?
// input value is elevation at comparison point
float View_Grid::getVerticalAngle(float row, float col, float value) {

	float distance = sqrt( pow(row - vp_row, 2) + pow(col - vp_col, 2) );
	return atan((value - elevGrid->getGridValueAt(this->getVProw(), this->getVPcol()))
				/ distance);
}

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