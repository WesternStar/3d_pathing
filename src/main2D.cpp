#include <stdlib.h> //Included for function exit()
#include <fstream>  //Included for ifstream class
#include <iostream> //Included for ostream class (cout)
#include <memory>
#include "../include/pgmioClass.hpp"
#include "../include/shape.h"

using namespace std;
std::unique_ptr<PGMData> Field;
inline float rint(float x) { return float((int)(x + .5)); }

// Delcaration of functions to read/write pgm images
//-------------------------------------------------

int readPGM(const char *filename, unsigned char *&image, int &xsize,
            int &ysize);
int writePGM(const char *filename, unsigned char *image, int xsize, int ysize);

// Declaration of function to compute "distance to destination" shape in 3D grid
//----------------------------------------------------------------------------
// obst = binary input obstacle array (xsize by ysize)
// dist = output array of distances (xsize by ysize)
// s   = shape object to be navigated through the obstacles
// dest = grid index of destination location (where dist = 0)

void computeDistances2D(unsigned char *obst, shape *s, int dest, int *dist,
                        int xsize, int ysize);

// Read rhombus parameters (width,height,angle) from file and pass them to
// object

void readAndSetRhombusParams(ifstream &fin, rhombus *myrhom) {
  float width, height, corner_angle;
  fin >> width;
  if (!fin.good()) {
    cout << "Invalid width\n";
    exit(1);
  }
  fin >> height;
  if (!fin.good()) {
    cout << "Invalid height\n";
    exit(1);
  }
  fin >> corner_angle;
  if (!fin.good()) {
    cout << "Invalid angle\n";
    exit(1);
  }
  myrhom->setParams(width, height, corner_angle);
}

// Read lens parameters (radius,arc_angle) from file and pass them to object

void readAndSetLensParams(ifstream &fin, lens *mylens) {
  float radius, arc_angle;
  fin >> radius;
  if (!fin.good()) {
    cout << "Invalid radius\n";
    exit(1);
  }
  fin >> arc_angle;
  if (!fin.good()) {
    cout << "Invalid arc_angle\n";
    exit(1);
  }
  mylens->setParams(radius, arc_angle);
}

// Read capsule parameters (width,height) from file and pass them to object

void readAndSetCapsuleParams(ifstream &fin, capsule *mycaps) {
  float width, height;
  fin >> width;
  if (!fin.good()) {
    cout << "Invalid width\n";
    exit(1);
  }
  fin >> height;
  if (!fin.good()) {
    cout << "Invalid height\n";
    exit(1);
  }
  mycaps->setParams(width, height);
}

// Read starting/destination location from file and return nearest grid point

int readAndSetLocation(ifstream &fin, shape *myshape, int xsize, int ysize) {
  float x, y, theta;
  fin >> x;
  if (!fin.good()) {
    cout << "invalid x-coordinate\n";
    exit(1);
  }
  fin >> y;
  if (!fin.good()) {
    cout << "invalid y-coordinate\n";
    exit(1);
  }
  fin >> theta;
  if (!fin.good()) {
    cout << "invalid rotation angle\n";
    exit(1);
  }
  x = rint(x);
  y = rint(y); // Igore theta from file (for this 2D version)
  myshape->setLocation(x, y, 0);
  return int(x + xsize * y);
}

int main(int argc, char *argv[]) {

  // Make sure number of command line arguments is correct

  if (argc <= 3) {
    cout << "Missing arguments. Should be: " << argv[0]
         << " obstacle.pgm output.pgm shape.txt" << endl;
    exit(1);
  }

  // Read input image (obstacles) and record grid dimensions
  PGMData image(argv[1]);
  // TODO Handle failure to load file this by thowing exception in PGMData and
  // catching it here

  // Total number of pixels in the grid
  int gridsize = image.xSize() * image.ySize();

  // Open shape input file

  ifstream fin;
  fin.open(argv[3]);

  if (!fin.is_open()) {
    cout << "Could not open shape file: " << argv[3] << endl;
    exit(1);
  }

  // Read from shape input file (single line)

  char shapetype;
  // Read first character from shape file
  fin >> shapetype;

  // Get shape parameters from input file

  rhombus myrhom;
  lens mylens;
  capsule mycaps; // Derived shape objects
  shape *myshape; // Base pointer to one of the above 3 derived shape objects

  switch (shapetype) {
  case 'R':
    myshape = &myrhom;
    readAndSetRhombusParams(fin, &myrhom);
    break;
  case 'L':
    myshape = &mylens;
    readAndSetLensParams(fin, &mylens);
    break;
  case 'C':
    myshape = &mycaps;
    readAndSetCapsuleParams(fin, &mycaps);
    break;
  default:
    cout << "Unrecognized shape type\n";
    exit(1);
  }

  // Get starting location and draw starting shape outline

  //int p1 = readAndSetLocation(fin, myshape, image.xSize(), image.ySize()); // Starting point
  auto start = readAndSetLocation(fin, myshape, image.xSize(), image.ySize()); // Starting point


  // TODO add in checks against isValid and drawOutline the field based function using points
  // Make sure it checks for whether the start location is valid

  // Get destination location and draw starting shape outline

  //int p2 = readAndSetLocation(fin, myshape, image.xSize(), image.ySize()); // Destination point
  auto end = readAndSetLocation(fin, myshape, image.xSize(), image.ySize()); // Destination point

     // Draw destination shape

  // Close the shape input file

  fin.close();

  // Compute "distance to destination" function at valid (x,y) locations

  int *dist = new int[gridsize]; // Allocate array for dist
  for (int p = 0; p < gridsize; p++)
    dist[p] = gridsize; // Initialize dist values

  //TODO Rewrite compute distances 2d

  //computeDistances2D(image, myshape, p2, dist, image.xsize, image.ysize);

  // Move shape from starting point to destination via the optimal path

  int steps = 0;
  int X = 1, Y = image.xSize(); // Offsets needed to move by one pixel

  int p1,p2;
  p2 = start; // Initialize neighbor point p2 to starting point p1

  do {
    p1 = p2; // Move p1 to smallest neighbor p2
    int j = p1 / Y, i = p1 - j * Y; // 2D indeces of current point

    // Periodically draw shape (every 15 steps) at the current location p1

    if (steps++ == 15) {
	    
      //myshape->setLocation(i, j, 0);
      //TODO replace the shape draw with the field draw
      //myshape->drawOutline(image, image.xSize(), ysize);
      steps = 0;
    }

    // Possibly move in x-direction

    if (i > 0 && dist[p2 - X] < dist[p2])
      p2 -= X;
    else if (i < image.xSize() - 1 && dist[p2 + X] < dist[p2])
      p2 += X;

    // Possibly move in y-direction

    if (j > 0 && dist[p2 - Y] < dist[p2])
      p2 -= Y;
    else if (j < image.ySize() - 1 && dist[p2 + Y] < dist[p2])
      p2 += Y;

  } while (p1 != p2);

  // Write output image

  //TODO write to fields
 // if (!writePGM(argv[2], image, image.xSize(), image.ySize()))
  //  cout << "Unable to write output image: " << argv[2] << endl;

  // Deallocate memory

  delete[] dist;

  return 0;
}
