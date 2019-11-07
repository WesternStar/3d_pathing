#include "pgmioClass.hpp"
#include "shape.h"
#include <fstream>  //Included for ifstream class
#include <iostream> //Included for ostream class (cout)
#include <stdlib.h> //Included for function exit()

using namespace std;

#define D_THETA 10 // Angle increment (degrees) between neighboring z-indeces

// Declaration of function to compute "distance to destination" shape in 3D grid
//----------------------------------------------------------------------------
// obst = binary input obstacle array (xsize by ysize)
// dist = output array of distances (xsize by ysize by zsize)
// s   = shape object to be navigated through the obstacles
// dest = grid index of destination location (where dist = 0)

int main(int argc, char *argv[]) {

  // Make sure number of command line arguments is correct

  if (argc <= 1) {
    cout << "Missing arguments. Should be: " << argv[0] << "output.pgm" << endl;
    exit(1);
  }

  // Read input image (obstacles) and record grid dimensions

  int xsize{360}, ysize{360}, zsize = 360 / D_THETA;
  PGMData image;
  image.createBlank(xsize, ysize);
  Line A;

  // TODO Draw A Box

  // Write output image
  image.writeData(argv[1]);
  if (!image.written)
    cout << "Unable to write output image: " << argv[1] << endl;

  return 0;
}
