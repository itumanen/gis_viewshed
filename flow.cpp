#include "fa_grid.h"
#include <cstring>
#include <time.h>

int main(int argc, char** argv) {

    //check args and argc to make sure the user entered a  file
    //name and a value on the command line

    if (argc < 2) {
        cout << "error, " << argc << " is an invalid number of inputs" << endl;
        return(EXIT_FAILURE);
    }

    string elevName, fdName, faName; 
    Grid* elevGrid;
    FD_Grid* fdGrid;
    FA_Grid* faGrid;

    clock_t timestamp = clock();
    
    // Original grid
    elevGrid = new Grid(argv[1]);
    elevGrid->printInfo();

    fdGrid = new FD_Grid(elevGrid);
    fdGrid->computeFlowDir(elevGrid);
    printf("\nFlow direction grid\n");

    char* outputFD = argv[2];
    fdGrid->writeGridToFile(outputFD);

    faGrid = new FA_Grid(elevGrid);
    printf("\nFlow accumulation grid\n");
    faGrid->computeFlow(fdGrid);

    char* outputFA = argv[3];
    faGrid->writeGridToFile(outputFA);

    timestamp = clock() - timestamp;
    printf("Timestamp is %lu\n", timestamp);
    printf("Run time = %f\n", (float)timestamp / CLOCKS_PER_SEC);

    // delete memory from array
    elevGrid->freeGridData();
    fdGrid->freeGridData();
    faGrid->freeGridData();

    return(EXIT_SUCCESS);

}