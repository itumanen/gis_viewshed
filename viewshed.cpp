#include "view_grid.h"
#include <cstring>
#include <time.h>

int main(int argc, char** argv) {

    //check args and argc to make sure the user entered a  file
    //name and a value on the command line

    if (argc < 5) {
        cout << "error, " << argc << " is an invalid number of inputs" << endl;
        return(EXIT_FAILURE);
    }

    // parse input 
    char* inputGrid = argv[1];
    char* outputView = argv[2];
    int vp_row = atoi(argv[3]);
    int vp_col = atoi(argv[4]);

    // declare grids
    Grid* elevGrid;
    View_Grid* viewGrid;
    
    // Original grid
    elevGrid = new Grid(inputGrid);
    elevGrid->printInfo();
    elevGrid->printGrid();

    // Viewshed grid
    viewGrid = new View_Grid(elevGrid, vp_row, vp_col);
    printf("viewGrid viewpoint at %d, %d\n", viewGrid->getVProw(), viewGrid->getVPcol());

    // Start timer
    clock_t timestamp = clock();

    // Compute viewshed grid
    viewGrid->computeViewshed();
    float tanX = viewGrid->interpolate(1,1);
    printf("tanX is %f\n", tanX);

    // End timer and print computation time
    timestamp = clock() - timestamp;
    printf("Run time = %f\n", (float)timestamp / CLOCKS_PER_SEC);

    // Write viewshed grid info to output file 
    viewGrid->writeGridToFile(outputView);

    return(EXIT_SUCCESS);

}