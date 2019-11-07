#include <stdio.h>
#include <string.h>
#include <vector>
#include <utility>
#include <iostream>
#include <memory>
#include <cstddef>

using std::vector;
using std::pair;
using std::unique_ptr;
using std::make_unique;
const int debug = 0;

class PGMData {
    int xsize;
    int ysize;
    vector<std::byte> image;
    FILE *Handle;

public:
    bool loaded = 0;
    bool written = 0;
    PGMData() { };
    ~PGMData() { }

    PGMData(const char *filename){
    char line[81], word[81];

    // Initialize image sizes and pointer

    xsize = ysize = 0;

    // Open input file for reading

    if (!(Handle = fopen(filename, "r"))) {
        loaded = 0;
        return;
    }

    // Read PGM header

    fgets(line, 81, Handle);
    sscanf(line, "%s", word);
    if (strcmp(word, "P5")) {
        fclose(Handle);
        loaded = 0;
        return;
    } // Check for PGM identifier

    do {
        fgets(line, 81, Handle);
        sscanf(line, "%s", word);
    } // Read past comments
    while (*word == '#');

    sscanf(line, "%d %d", &xsize, &ysize); // Read image size

    fgets(line, 81, Handle); // Read past rest of header

    if (xsize <= 0 || ysize <= 0) {
        fclose(Handle);
        xsize = ysize = 0;
        loaded = 0;
        return;
    }

    // Allocate space for image data

    image.resize(xsize * ysize);

    // Read image data from file

    if (fread(image.data(), sizeof(unsigned char), xsize * ysize, Handle) <
        xsize * ysize) {
        fclose(Handle);
        xsize = ysize = 0;
        loaded = 0;
        return;
    }

    // Close file and return

    fclose(Handle);
    loaded = 1;
    return;
}

    void createBlank(int x, int y){
      xsize=x;
      ysize=y;
      image.resize(xsize * ysize);
      for (auto & a : image){
        a = std::byte{255};
      }
    }

    void writeData(const char *filename){

    // Open output file for writing

    if (!(Handle = fopen(filename, "w"))) {
        throw;
        return;
    }

    // Write PGM header

    fprintf(Handle, "P5\n");
    fprintf(Handle, "# CREATOR: pgmio.C Rev: 11/04/01\n");
    fprintf(Handle, "%d %d\n", xsize, ysize);
    fprintf(Handle, "255\n");

    // Write image data to file

    fwrite(image.data(), sizeof(std::byte), xsize * ysize, Handle);

    // Close file and return

    fclose(Handle);
    written = 1;
    return;
}

    bool IsValidLocation(int x, int y){
    if (debug) {
      int opac = std::to_integer<int>(image[xsize * y + x]);
        std::cout << "IVL: Opacity" << opac << std::endl;
        bool valid = opac != 0;
        if (valid)
            std::cout << "IVL:Valid Location\n";
        else
            std::cout << "IVL:Invalid Location\n";
        return valid;
    } else {

      return std::to_integer<int>(image[xsize * y + x]) != 0;
    }
}

    bool AreValidLocations(vector<pair<int, int>> &points){
    for (auto i : points) {
      bool valid = IsValidLocation(i.first, i.second);
      if(!valid){
        if (debug){
          std::cout << "AVL:Invalid Location\n";
        }
        return false;
      }
    }
    if (debug)
        std::cout << "AVL:Valid Location\n";
    return true;
    }

    void DrawPoint(int x, int y, int opacity){
    size_t location = xsize * y + x;
    image[location] = std::byte(opacity);
}

    void DrawPoints(vector<pair<int, int>> &points, int opacity){
    for (auto i : points) {
        DrawPoint(i.first, i.second, opacity);
    }
}

    void Print(){
    for (int y = 0; y < ysize; y++) {
        for (int x = 0; x < xsize; x++) {
          if (std::to_integer<int>(image[y * xsize + x]) < 64) {
                std::cout << "*";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}

    int xSize() { return xsize; }

    int ySize() { return ysize; }
};








