#include "pgmioClass.hpp"
#include "shape.h"
#include <fstream>  //Included for ifstream class
#include <iostream> //Included for ostream class (cout)
#include <memory>
#include <stdlib.h> //Included for function exit()

using namespace std;
std::unique_ptr<PGMData> Field;

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

void computeDistances2D(PGMData *obst, shape *s, int seed, int *dist, int xsize,
                        int ysize);

// Read rhombus parameters (width,height,angle) from file and pass them to
// object

tuple<float, float, float> readAndSetRhombusParams(ifstream &fin) {
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
  return make_tuple(width, height, corner_angle);
}

// Read lens parameters (radius,arc_angle) from file and pass them to object

tuple<float, float> readAndSetLensParams(ifstream &fin) {
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
  return make_tuple(radius, arc_angle);
}

// Read capsule parameters (width,height) from file and pass them to object

tuple<float, float> readAndSetCapsuleParams(ifstream &fin) {
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
return make_tuple(width, height);
}

// Read starting/destination location from file and return nearest grid point

tuple<float, float, float> readAndSetLocation(ifstream &fin)  {
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
  cout << "Loaded Image" << "\n";
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
  // Data format is as follows
  //  ST X Y θ PAR* X2 Y2 θ2
  // Single Character Shapetype followed by
  // Starting X, Y and Theta and optional parameters followed by the ending x y θ

  char shapetype;
  // Read first character from shape file
  fin >> shapetype;

  cout << "Read shapetype:"<< shapetype << "\n";
  // Get shape parameters from input file

  shape *myshape; // Base pointer to one of the above 3 derived shape objects
  unique_ptr<rhombus> myrhom;
  unique_ptr<lens> mylens;
  unique_ptr<capsule> mycaps; // Derived shape objects

  auto[x, y, theta] = readAndSetLocation(fin) ; // Starting point
  switch (shapetype) {
  case 'R': {
 auto [width, height, corner_angle] =
        readAndSetRhombusParams(fin);
    myrhom = make_unique<rhombus>(x, y, theta, width, height, corner_angle);
    myshape = myrhom.get();
  } break;
  case 'L': {
    auto[radius, arc_angle] = readAndSetLensParams(fin);
    mylens = make_unique<lens>(x, y, theta, radius, arc_angle);
    myshape = mylens.get();
  } break;
  case 'C': {
    auto[width, height] = readAndSetCapsuleParams(fin);
    mycaps = make_unique<capsule>(x, y, theta, width, height);
    myshape = mycaps.get();
  } break;
  default:
    cout << "Unrecognized shape type\n";
    exit(1);
  }
  auto [x2,y2,theta2] = readAndSetLocation(fin);

  auto draw = [&myshape, &image]() {
                image.drawPointsAtLoc(myshape->getPoints(), 68, myshape->relative);
  };
  // Get starting location and draw starting shape outline
  // Starting point
  auto start = int(x + image.xSize() * y);
  draw();
  cout << "Drew Starting Location\n";
  // return

  // Get destination location and draw starting shape outline
  // Destination point
  auto end = int(x2 + image.xSize() * y2);
  myshape->setLocation(x2, y2, theta2);
  draw();
  cout << "Drew Ending Location\n";

  // Close the shape input file
  fin.close();

  // Compute "distance to destination" function at valid (x,y) locations

  cout << "Read all parameters" << "\n";

  int *dist = new int[gridsize]; // Allocate array for dist
  for (int p = 0; p < gridsize; p++)
    dist[p] = gridsize; // Initialize dist values

  cout << "Initialized Distance Array" << "\n";
  computeDistances2D(&image, myshape, end, dist, image.xSize(), image.ySize());
  cout << "Computed Distances" << "\n";

  // Move shape from starting point to destination via the optimal path

  int steps = 0;
  int X = 1, Y = image.xSize(); // Offsets needed to move by one pixel

  int p1, p2;
  p2 = start; // Initialize neighbor point p2 to starting point p1

  do {
    p1 = p2;                        // Move p1 to smallest neighbor p2
    int j = p1 / Y, i = p1 - j * Y; // 2D indeces of current point

    // Periodically draw shape (every 15 steps) at the current location p1

    if (steps++ == 15) {

      myshape->setLocation(i, j, 0);
      draw();
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
  cout << "Printed Path" << "\n";
  // Write output image

  image.writeData(argv[2]);
  if (!image.written)
    cout << "Unable to write output image: " << argv[1] << "\n";
  cout << "Wrote image" << "\n";

  // Deallocate memory
  delete[] dist;

  return 0;
}
