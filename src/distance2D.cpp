#include "pgmioClass.hpp"
#include "shape.h"
#include <string.h> //for memset()
#include <vector>
#include <queue>
void computeDistances2D(PGMData *obst, shape *s, int seed, int *dist, int xsize,
                        int ysize)
// obst = binary input obstacle array (xsize by ysize)
// dist = output array of distances (xsize by ysize)
// s   = shape object to be navigated through the obstacles
// seed = grid index of destination location (where distance = 0)
{
  int p, q;                     // Index of current point and neighbor
  int i, j;                     // 2D offsets of current point
  int gridsize = xsize * ysize; // Total size of grid

  const int X = 1;     // Offset equivalent to 1 step in x direction
  const int Y = xsize; // Offset equivalent to 1 step in y direction

  auto valid = [&obst, &s](int x, int y) {
    s->setLocation(x, y, 0);
    auto a = s->getPoints();
    return obst->areValidLocationsAtLoc(a,s->rel());
  };
  vector<int> visited(gridsize, 0);

  queue<int> queue; // Queue used to determine visiting order

  // Initialize arrays and queue

  dist[seed] = 0;
  visited[seed] = 1;
  queue.push(seed);

  // Build distance function by marching outwards from seed

  while (!queue.empty()) {
    p=queue.front();
    j = p / Y;
    i = p - j * Y; // Convert 1D index to 2D indices

    if (i > 0 && !visited[q = p - X]) { // Visit -X neighbor
      assert(!visited[q]);
      visited[q] = 1;
      if (valid(i - 1, j)) {
        dist[q] = dist[p] + 1; // queue.append(q);}
        queue.push(q);
      }
    }

    if (i < xsize - 1 && !visited[q = p + X]) { // Visit +X neighbor
      assert(!visited[q]);
      visited[q] = 1;
      if (valid(i + 1, j)) {
        dist[q] = dist[p] + 1; // queue.append(q);}
        queue.push(q);
      }
    }

    if (j > 0 && !visited[q = p - Y]) { // Visit -Y neighbor
      assert(!visited[q]);
      visited[q] = 1;
      if (valid(i, j - 1)) {
        dist[q] = dist[p] + 1; // queue.append(q);}
        queue.push(q);
      }
    }

    if (j < ysize - 1 && !visited[q = p + Y]) { // Visit +Y neighbor
      assert(!visited[q]);
      visited[q] = 1;
      if (valid(i, j + 1)) {
        dist[q] = dist[p] + 1; // queue.append(q);}
        queue.push(q);
      }
    }
    queue.pop();
  }
}
