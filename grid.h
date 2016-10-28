#include <cstdio>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <assert.h>

using namespace std;

#define NODATA "NODATA_value"
#define ncols "ncols"
#define nrows "nrows"
#define xllcorner "xllcorner"
#define yllcorner "yllcorner"
#define cellsize "cellsize"
#define DEFAULT_NODATA -99999
#define INITIAL_VALUE 1

class Grid {

public:
	Grid(); // Default constructor

	Grid(char* fileName); // pass in file name to build the grid

	// grid info
	void printGrid();
	void printInfo();
	void readInGridFile(char* fileName);
	void multiply(float factor);
	void freeGridData();

	void writeGridToFile(char* fileName);

	// getters
	int getNumCols() { return this->numCols; }
	int getNumRows() { return this->numRows; }
	float getXllCorner() { return this-> xllCorner; }
	float getYllCorner() { return this->yllCorner; }
	float getCellSize() { return this->cellSize; }
	float getMinVal() { return this->minVal; }
	float getMaxVal() { return this->maxVal; }
	float getNodataValue() { return this->nodata_value; }

	float** getGridValues() {return this->gridVals; }
	float getGridValueAt(int i, int j) { return this->gridVals[i][j]; }
	bool isNoData(int i, int j) { return (this->getGridValueAt(i,j) == this->nodata_value); }
	float* getRow(int row) {return this->gridVals[row]; }

	// setters
	void setNumCols(int numCols) { this->numCols = numCols; }
	void setNumRows(int numRows) { this->numRows = numRows; }
	void setXllCorner(float xllCorner) { this->xllCorner = xllCorner; }
	void setYllCorner(float yllCorner) { this->yllCorner = yllCorner; }
	void setCellSize(float cellSize) { this->cellSize = cellSize; }
	void setMinVal(float minVal) { this->minVal = minVal; }
	void setMaxVal(float maxVal) { this->maxVal = maxVal; }
	void setNodataValue(float nodata_value) {this->nodata_value = nodata_value; }
	void setGridValueAt(int i, int j, float value) {this->gridVals[i][j] = value; }
	float** setGridVals();

	// ~Grid(); 

protected:

	int numCols;
	int numRows;

	float xllCorner;
	float yllCorner;

	float cellSize;

	float minVal;
	float maxVal;

	float nodata_value;

	float** gridVals;

};

