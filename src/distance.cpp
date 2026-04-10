#include "pgmioClass.hpp"
#include "shape.h"
#include <vector>
#include <queue>


void computeDistances
(PGMData *obst, shape *s, int seed, int *dist, int xsize, int ysize, int zsize)
// obst = binary input obstacle array (xsize by ysize)
// dist = output array of distances (xsize by ysize)
// s    = shape object to be navigated through the obstacles
// seed = grid index of destination location (where distance = 0)
{
  int p, q;                             // Index of current point and neighbor
  int i, j, k;                          // 3D offsets of current point
  int gridsize = xsize * ysize * zsize; // Total size of grid

  const int X = 1;             // Offset equivalent to 1 step in x direction
  const int Y = xsize;         // Offset equivalent to 1 step in y direction
  const int Z = xsize * ysize; // Offset equivalent to 1 step in z direction

  auto valid = [&](int x, int y, int z) {
    s->setLocation(x, y, z * 10);
    auto pts = s->getPoints();
    return obst->areValidLocationsAtLoc(pts, s->rel());
  };

  std::vector<int> visited(gridsize, 0);
  std::queue<int> queue; // Queue used to determine visiting order

  // Initialize arrays and queue

  dist[seed] = 0;
  visited[seed] = 1;
  queue.push(seed);

  // Build distance function by marching outwards from seed

  while (!queue.empty()) {
    p = queue.front();
    k = p / Z; j = (p - k * Z) / Y; i = p - k * Z - j * Y; // Convert 1D index to 3D indices

    if (i > 0 && !visited[q = p - X]) {         // Visit -X neighbor
      visited[q] = 1;
      if (valid(i - 1, j, k)) { dist[q] = dist[p] + 1; queue.push(q); }
    }

    if (i < xsize - 1 && !visited[q = p + X]) { // Visit +X neighbor
      visited[q] = 1;
      if (valid(i + 1, j, k)) { dist[q] = dist[p] + 1; queue.push(q); }
    }

    if (j > 0 && !visited[q = p - Y]) {         // Visit -Y neighbor
      visited[q] = 1;
      if (valid(i, j - 1, k)) { dist[q] = dist[p] + 1; queue.push(q); }
    }

    if (j < ysize - 1 && !visited[q = p + Y]) { // Visit +Y neighbor
      visited[q] = 1;
      if (valid(i, j + 1, k)) { dist[q] = dist[p] + 1; queue.push(q); }
    }

    if (k > 0 && !visited[q = p - Z]) {                          // Visit -Z neighbor
      visited[q] = 1;
      if (valid(i, j, k - 1)) { dist[q] = dist[p] + 1; queue.push(q); }
    } else if (k == 0 && !visited[q = p + Z * (zsize - 1)]) {   // Wrap -Z to far end
      visited[q] = 1;
      if (valid(i, j, zsize - 1)) { dist[q] = dist[p] + 1; queue.push(q); }
    }

    if (k < zsize - 1 && !visited[q = p + Z]) {                  // Visit +Z neighbor
      visited[q] = 1;
      if (valid(i, j, k + 1)) { dist[q] = dist[p] + 1; queue.push(q); }
    } else if (k == zsize - 1 && !visited[q = p - Z * (zsize - 1)]) { // Wrap +Z to near end
      visited[q] = 1;
      if (valid(i, j, 0)) { dist[q] = dist[p] + 1; queue.push(q); }
    }

    queue.pop();
  }
}
