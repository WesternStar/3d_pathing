#include "pgmioClass.hpp"
#include "shape.h"
#include <assert.h>
#include <fstream>  //Included for ifstream class
#include <iostream> //Included for ostream class (cout)
#include <stdlib.h> //Included for function exit()

using namespace std;

#define D_THETA 10 // Angle increment (degrees) between neighboring z-indices

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
  image.drawBlank(xsize, ysize);

  try {
    // Draw A Box
    rhombus box(90, 180, 0, 180, 90, 90);
    image.drawPoints(box.getPoints(), 128);

    cout << "Drew Box\n";
    // Draw a Lens
    lens K(40, 180, 0, 30, 50);
    cout << K.getPoints().size();
    image.drawPoints(K.getPoints(), 64);
    // Draw a Capsule
    capsule Cap(90,300,0,10,10);
    image.drawPoints(Cap.getPoints(), 64);
    // Write output image
    image.writeData(argv[1]);
    if (!image.written)
      cout << "Unable to write output image: " << argv[1] << endl;
  } catch (const std::bad_alloc &e) {
    std::cout << "Allocation failed: " << e.what() << '\n';
  }

  return 0;
}
