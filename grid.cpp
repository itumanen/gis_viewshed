#include "grid.h"

// Default constructor
Grid::Grid() { 

	this->numCols = 0;
	this->numRows = 0;

	this->xllCorner = 0.0;
	this->yllCorner = 0.0;

	this->cellSize = 0.0;

	this->minVal = 0.0;
	this->maxVal = 0.0;

	this->nodata_value = DEFAULT_NODATA;

	float** gridVals;
	
}


// Constructor
// Takes in pointer to a file name and reads in file
// Calls readInGridFile; sets values of attributes
Grid::Grid(char* fileName) {
	readInGridFile(fileName);
}

// Iterates through the 2D grid array and prints values
void Grid::printGrid(void) {

	for (int row = 0; row < this->numRows; row++) {
		for (int col = 0; col < this->numCols; col++) {
			cout << this->gridVals[row][col] << "\t";
		}
		cout << "\n" << endl;
	}
}

// Prints first six lines of the grid file and attributes
// of the grid
void Grid::printInfo(void) {
	printf("\nncols\t\t%d\n", this->numCols);
	printf("nrows\t\t%d\n", this->numRows);
	printf("xllcorner\t%f\n", this->xllCorner);
	printf("yllcorner\t%f\n", this->yllCorner);
	printf("cellsize\t%f\n", this->cellSize);
	printf("NODATA_value\t%f\n\n", this->nodata_value);
}

// Takes in pointer to array of characters (file name)
// Reads in contents of the file and sets attributes of the grid class
void Grid::readInGridFile(char* fileName) {

	FILE* f;
	char buffer[100];
	char valBuffer[100];

	f = fopen(fileName, "r");
	
	if (f == NULL) {
		cerr << "Error: file is empty!" << endl;
		exit(1);
	}

	// read in column data
	fscanf(f, "%s", buffer);
	if (strcmp(buffer, ncols) != 0) {
		cout << "read in " << buffer << endl;
		cerr << "Error reading col data" << endl;
		exit(1);
	}
	
	fscanf(f, "%s", buffer);
	setNumCols(atoi(buffer)); 

	// read in row data
	fscanf(f, "%s", buffer);
	if(strcmp(buffer, nrows) != 0) {
		cerr << "Error reading row data" << endl;
		exit(1);
	}
	fscanf(f, "%s", buffer);
	setNumRows(atoi(buffer));

	//read in xll corner data
	fscanf(f, "%s", buffer);
	if(strcmp(buffer, xllcorner) != 0) {
		cerr << "Error reading xll corner data" << endl;
		exit(1);
	}
	fscanf(f, "%s", buffer);
	setXllCorner(atof(buffer));

	// read in yll corner data
	fscanf(f, "%s", buffer);
	if(strcmp(buffer,yllcorner) != 0) {
		cerr << "Error reading yll corner data" << endl;
		exit(1);
	}
	fscanf(f, "%s", buffer);
	setYllCorner(atof(buffer));

	//read in cell size
	fscanf(f, "%s", buffer);
	if(strcmp(buffer,cellsize) != 0) {
		cerr << "Error reading cell size data" << endl;
		exit(1);
	}
	fscanf(f, "%s", buffer);
	setCellSize(atof(buffer));

	// read in NODATA value
	fscanf(f, "%s", buffer);
	if(strcmp(buffer, NODATA) != 0) {
		cerr << "Error reading NODATA_value" << endl;
		exit(1);
	}
	fscanf(f, "%s", buffer);
	setNodataValue(atof(buffer));

	// allocate array
	this->gridVals = (float**) malloc(sizeof(float*) * this->numRows);
	assert(this->gridVals);
	for (int i = 0; i < this->numRows; i++) {
		this->gridVals[i] = (float*) malloc(sizeof(float) * this->numCols);
		assert(this->gridVals[i]);
	}

	// read in array values
	for (int i = 0; i < this->numRows; i++) {
		for (int j = 0; j < this->numCols; j++) {
			fscanf(f, "%s", buffer);
			this->gridVals[i][j] = atof(buffer);
		}
	}

	fclose(f);

}

// Multiplies grid values by input float 
void Grid::multiply(float factor) {

	for (int row = 0; row < this->numRows; row++) {
		for (int col = 0; col < this->numCols; col++) {
	
			if (this->gridVals[row][col] == this->nodata_value) {
				continue;
			}
	
			float value = this->gridVals[row][col];
			this->gridVals[row][col] = value * factor;
	
		}
	}

}

// Given input file name, write out the grid info and the grid to file
void Grid::writeGridToFile(char* fileName) {
	
	ofstream outputFile;
	outputFile.open(fileName);

	outputFile << "ncols " << this->numCols << "\n";
	outputFile << "nrows " << this->numRows << "\n";
	outputFile << "xllcorner " << this->xllCorner << "\n";
	outputFile << "yllcorner " << this->yllCorner << "\n";
	outputFile << "cellsize " << this->cellSize << "\n";
	outputFile << "NODATA_value " << this->nodata_value << "\n\n";
	
	for (int row = 0; row < this->numRows; row++) {
		for (int col = 0; col < this->numCols; col++) {
			outputFile << this->gridVals[row][col] << "  ";
		}
		outputFile << "\n";
	}
	outputFile << "\n";
	outputFile.close();

}

// Frees memory allocated by grids
void Grid::freeGridData() {
	free(this->getGridValues());
}

// Grid::~Grid() {
// 	this->freeGridData();
// }

