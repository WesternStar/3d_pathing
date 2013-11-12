#include <stdio.h>
#include <string.h>
#include <vector>
#include <utility>
class PGMData{
 int xsize;
 int ysize;
 unsigned char *image=nullptr;
 FILE * Handle;
 bool loaded=0;
 bool writen=0;
 public:
  PGMData();
  PGMData(const char * filename);
  ~PGMData;
  writeData(const char * filename);
  bool IsValidLocation(int ,int);
  bool AreValidLocations(vector<pair<int,int>>);
  void DrawPoint(int,int,int);
  void DrawPoints(vector<pair<int,int>>,int);
  void Print();
}
PGMData::PGMData(const char * filename){
  char line[81],word[81];

  //Initialize image sizes and pointer

  image=nullptr; xsize=ysize=0;

  //Open input file for reading

  if (!(Handle=fopen(filename,"r"))) {loaded=0; return;}

  //Read PGM header

  fgets(line,81,Handle); sscanf(line,"%s",word);
  if (strcmp(word,"P5")) {fclose(Handle); loaded=0; return;}   //Check for PGM identifier

  do {fgets(line,81,Handle); sscanf(line,"%s",word);}  //Read past comments
  while (*word=='#');

  sscanf(line,"%d %d",&xsize,&ysize);                //Read image size

  fgets(line,81,Handle);                               //Read past rest of header

  if (xsize<=0 || ysize<=0) {fclose(Handle); xsize=ysize=0; loaded=0; return;}

  //Allocate space for image data

  image=new unsigned char[xsize*ysize];

  //Read image data from file

  if (fread(image,sizeof(unsigned char),xsize*ysize,Handle)<xsize*ysize)  {
    fclose(Handle); delete[] image; image=NULL; xsize=ysize=0; loaded=0 return;
  }

  //Close file and return

  fclose(Handle);
  loaded=1;
  return ;
}

int PGMData::writeData(const char *filename) {

  //Open output file for writing

  if (!(Handle=fopen(filename,"w"))){writen=0; return;}

  //Write PGM header

  fprintf(Handle,"P5\n");
  fprintf(Handle,"# CREATOR: pgmio.C Rev: 11/04/01\n");
  fprintf(Handle,"%d %d\n",xsize,ysize);
  fprintf(Handle,"255\n");

  //Write image data to file

  fwrite(image,sizeof(unsigned char),xsize*ysize,Handle);

  //Close file and return

  fclose(Handle);
  writen=1;
  return ;
}
PGMData::~PGMData(){
  free(image);
}

bool PGMData::IsValidLocation(int x ,int y){
  return image[ysize*y+x]!=255;
  
}
bool PGMData::AreValidLocations(vector<pair<int,int>> points){
  for( auto i :points){
    if (IsValidLocation(i.first,i.second)){
    }else{
      return false;
    }
  }
  return true;
}
void PGMData::DrawPoints(vector<pair<int,int>> points,int scale){
  for( auto i : points){
    DrawPoint(i.first,i.second,scale);
  }
  
}
void PGMData::DrawPoint(int x, int y, int scale){
  image[ysize*y+x]=scale;
  
}

void Print(){
  for (int y=0;y<ysize;y++){
    for(int x=0;x<xsize;x++){
      cout << image[y*ysize+x];
    }
    cout << "\n";
  }
}
