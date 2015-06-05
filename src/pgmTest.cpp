#include "pgmioClass.hpp"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <utility>
#include <iostream>
using std::vector;
using std::pair;
#include <stdio.h>
int main( int argc, const char* argv[] )
{
        vector<pair<int,int>> points{{20,15},{19,15},{18,15},{17,15},{16,15},{10,15},{11,15},{12,15},{13,15},{14,15},{15,15},{15,20},{15,19},{15,18},{15,17},{15,16},{15,10},{15,11},{15,12},{15,13},{15,14}};
	printf( "\nHello World\n\n" );
        PGMData Field("obst.pgm");
        Field.DrawPoints(points,0); 
        if(Field.IsValidLocation(15,15))
          std::cout << "Valid Location\n";
        else 
          std::cout << "Invalid Location\n";
        vector<pair<int,int>> line;
        for(int i =0; i<250;i++){
          line.emplace_back(i,95);
        }
        if(Field.AreValidLocations(line))        
          std::cout << "Valid Location\n";
        else 
          std::cout << "Invalid Location\n";

        Field.writeData("out.pgm");
  
}
