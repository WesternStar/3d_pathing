#include "pgmioClass.hpp"
#include "shape.h"
#include <fstream>   // Included for ifstream class
#include <iostream>  // Included for ostream class (cout)
#include <memory>
#include <stdlib.h>  // Included for function exit()

using namespace std;

#define D_THETA  10    // Angle increment (degrees) between neighboring z-indices

// Declaration of function to compute "distance to destination" shape in 3D grid
//------------------------------------------------------------------------------
// obst = binary input obstacle array (xsize by ysize)
// dist = output array of distances (xsize by ysize by zsize)
// s    = shape object to be navigated through the obstacles
// dest = grid index of destination location (where dist = 0)

void computeDistances(
  PGMData *obst, shape *s, int dest, int *dist, int xsize, int ysize, int zsize
);

// Read rhombus parameters (width,height,angle) from file and return as tuple

tuple<float, float, float> readAndSetRhombusParams(ifstream &fin) {
  float width, height, corner_angle;
  fin >> width;        if (!fin.good()) { cout << "Invalid width\n";  exit(1); }
  fin >> height;       if (!fin.good()) { cout << "Invalid height\n"; exit(1); }
  fin >> corner_angle; if (!fin.good()) { cout << "Invalid angle\n";  exit(1); }
  return make_tuple(width, height, corner_angle);
}

// Read lens parameters (radius,arc_angle) from file and return as tuple

tuple<float, float> readAndSetLensParams(ifstream &fin) {
  float radius, arc_angle;
  fin >> radius;    if (!fin.good()) { cout << "Invalid radius\n";    exit(1); }
  fin >> arc_angle; if (!fin.good()) { cout << "Invalid arc_angle\n"; exit(1); }
  return make_tuple(radius, arc_angle);
}

// Read capsule parameters (width,height) from file and return as tuple

tuple<float, float> readAndSetCapsuleParams(ifstream &fin) {
  float width, height;
  fin >> width;  if (!fin.good()) { cout << "Invalid width\n";  exit(1); }
  fin >> height; if (!fin.good()) { cout << "Invalid height\n"; exit(1); }
  return make_tuple(width, height);
}

// Read starting/destination location from file and return (x, y, theta)

tuple<float, float, float> readAndSetLocation(ifstream &fin) {
  float x, y, theta;
  fin >> x;     if (!fin.good()) { cout << "invalid x-coordinate\n";   exit(1); }
  fin >> y;     if (!fin.good()) { cout << "invalid y-coordinate\n";   exit(1); }
  fin >> theta; if (!fin.good()) { cout << "invalid rotation angle\n"; exit(1); }
  while (theta <   0) theta += 360; // Represent theta angle
  while (theta >= 360) theta -= 360; // between 0 and 360 deg.
  x = rint(x); y = rint(y); theta = rint(theta / D_THETA);
  return make_tuple(x, y, theta);
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
  cout << "Loaded image\n";
  // TODO: handle failure by throwing exception in PGMData and catching here

  int zsize = 360 / D_THETA;
  int gridsize = image.xSize() * image.ySize() * zsize; // Total number of pixels in the grid

  // Open shape input file

  ifstream fin;
  fin.open(argv[3]);

  if (!fin.is_open()) {
    cout << "Could not open shape file: " << argv[3] << endl;
    exit(1);
  }

  // Read from shape input file
  // Data format: ST X Y θ PAR* X2 Y2 θ2
  // Single character shapetype, starting X/Y/Theta, optional params, ending X/Y/Theta

  char shapetype;
  fin >> shapetype;
  cout << "Read shapetype: " << shapetype << "\n";

  // Get shape parameters from input file

  shape *myshape; // Base pointer to one of the derived shape objects
  unique_ptr<rhombus> myrhom;
  unique_ptr<lens>    mylens;
  unique_ptr<capsule> mycaps;

  auto [x, y, theta] = readAndSetLocation(fin); // Starting point

  switch (shapetype) {
  case 'R': {
    auto [width, height, corner_angle] = readAndSetRhombusParams(fin);
    myrhom = make_unique<rhombus>(x, y, theta * D_THETA, width, height, corner_angle);
    myshape = myrhom.get();
  } break;
  case 'L': {
    auto [radius, arc_angle] = readAndSetLensParams(fin);
    mylens = make_unique<lens>(x, y, theta * D_THETA, radius, arc_angle);
    myshape = mylens.get();
  } break;
  case 'C': {
    auto [width, height] = readAndSetCapsuleParams(fin);
    mycaps = make_unique<capsule>(x, y, theta * D_THETA, width, height);
    myshape = mycaps.get();
  } break;
  default:
    cout << "Unrecognized shape type\n";
    exit(1);
  }

  auto [x2, y2, theta2] = readAndSetLocation(fin); // Destination point

  auto draw = [&myshape, &image]() {
    image.drawPointsAtLoc(myshape->getPoints(), 68, myshape->relative);
  };

  auto isValid = [&myshape, &image]() {
    auto pts = myshape->getPoints();
    return image.areValidLocationsAtLoc(pts, myshape->rel());
  };

  // Get starting location and draw starting shape outline

  auto start = int(x + image.xSize() * (y + image.ySize() * theta));

  if (isValid()) {
    draw();
    cout << "Drew starting location\n";
  } else {
    cout << "Illegal start location." << endl;
    exit(1);
  }

  // Get destination location and draw destination shape outline

  auto end = int(x2 + image.xSize() * (y2 + image.ySize() * theta2));
  myshape->setLocation(x2, y2, theta2 * D_THETA);

  if (isValid()) {
    draw();
    cout << "Drew destination location\n";
  } else {
    cout << "Illegal destination." << endl;
    exit(1);
  }

  // Close the shape input file

  fin.close();

  // Compute "distance to destination" function at valid (x,y,theta) locations

  cout << "Read all parameters\n";

  int *dist = new int[gridsize];                      // Allocate array for dist
  for (int p = 0; p < gridsize; p++) dist[p] = gridsize; // Initialize dist values

  cout << "Initialized distance array\n";
  computeDistances(&image, myshape, end, dist, image.xSize(), image.ySize(), zsize);
  cout << "Computed distances\n";

  // Move shape from starting point to destination via the optimal path

  int steps = 0;
  int X = 1, Y = image.xSize(), Z = image.xSize() * image.ySize(); // Offsets needed to move by one pixel

  int p1, p2;
  p2 = start; // Initialize neighbor point p2 to the starting point

  do {
    p1 = p2;                                      // Move p1 to smallest neighbor p2
    int k = p1 / Z, j = (p1 - k * Z) / Y, i = p1 - k * Z - j * Y; // 3D indices

    // Periodically draw shape (every 15 time steps) at the current point p1

    if (steps++ == 15) {
      myshape->setLocation(i, j, k * D_THETA);
      draw();
      steps = 0;
    }

    // Possibly move in x-direction

    if (i > 0                  && dist[p2 - X] < dist[p2]) p2 -= X; else
    if (i < image.xSize() - 1  && dist[p2 + X] < dist[p2]) p2 += X;

    // Possibly move in y-direction

    if (j > 0                  && dist[p2 - Y] < dist[p2]) p2 -= Y; else
    if (j < image.ySize() - 1  && dist[p2 + Y] < dist[p2]) p2 += Y;

    // Possibly move in z-direction (rotation by D_THETA)

    if (k > 0        && dist[p2 - Z]          < dist[p2]) p2 -= Z;          else
    if (k < zsize-1  && dist[p2 + Z]          < dist[p2]) p2 += Z;          else
    if (k == 0       && dist[p2 - Z + gridsize] < dist[p2]) p2 += gridsize - Z; else
    if (k == zsize-1 && dist[p2 + Z - gridsize] < dist[p2]) p2 -= gridsize - Z;

  } while (p1 != p2);

  cout << "Printed path\n";

  // Write output image

  image.writeData(argv[2]);
  if (!image.written)
    cout << "Unable to write output image: " << argv[2] << endl;

  cout << "Wrote image\n";

  // Deallocate memory

  delete[] dist;

  return 0;
}
