#include <string.h>   //for memset()
//#include "list.h"
#include "shape.h"
#include <vector>
void computeDistances2D
(unsigned char *obst,shape *s,int seed,int *dist,int xsize,int ysize)
//obst = binary input obstacle array (xsize by ysize)
//dist = output array of distances (xsize by ysize)
// s   = shape object to be navigated through the obstacles
//seed = grid index of destination location (where distance = 0)
{
  int p,q;                          //Index of current point and neighbor
  int i,j;                          //2D offsets of current point
  int gridsize=xsize*ysize;         //Total size of grid

  const int X=1;                    //Offset equivalent to 1 step in x direction
  const int Y=xsize;                //Offset equivalent to 1 step in y direction

  //Initialize visited flags and queue

  int *visited=new int[gridsize];    //Marks which points have been visited

  vector<int> queue(gridsize);         //Queue used to determine visiting order
//  list<int> queue(gridsize);         //Queue used to determine visiting order

  //Initialize arrays and queue

  memset(visited,0,gridsize*sizeof(int));

  dist[seed]=0;
  visited[seed]=1;
  //queue.append(seed);
  queue.push_back(seed);

  //Build distance function by marching outwards from seed

  #define VALID(i,j) \
  (s->setLocation(i,j,0), s->isValidLocation(obst,xsize,ysize))

  //queue.rewind();
  queue.begin();
  // while (queue.getNext(p)) {
  for(auto it= queue.begin();it!=queue.end();it++){ 
    p=*it;
    j=p/Y; i=p-j*Y;                       //Convert 1D index to 2D indeces

    if (i>0 && !visited[q=p-X]) {         //Visit -X neighbor
      visited[q]=1;
      if (VALID(i-1,j)) {dist[q]=dist[p]+1; //queue.append(q);}
      queue.push_back(q);}
    }

    if (i<xsize-1 && !visited[q=p+X]) {   //Visit +X neighbor
      visited[q]=1;
      if (VALID(i+1,j)) {dist[q]=dist[p]+1;// queue.append(q);}
      queue.push_back(q);}
    }

    if (j>0 && !visited[q=p-Y]) {         //Visit -Y neighbor
      visited[q]=1;
      if (VALID(i,j-1)) {dist[q]=dist[p]+1; //queue.append(q);}
      queue.push_back(q);}
    }

    if (j<ysize-1 && !visited[q=p+Y]) {   //Visit +Y neighbor
      visited[q]=1;
      if (VALID(i,j+1)) {dist[q]=dist[p]+1;// queue.append(q);}
      queue.push_back(q);}
    }
  }

  #undef VALID

  delete[] visited;
}
