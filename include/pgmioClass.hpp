#include <stdio.h>
#include <string.h>
#include <vector>
#include <utility>
#include <iostream>
#include <memory>

using std::vector;
using std::pair;
using std::unique_ptr;
using std::make_unique;
const int debug = 1;

class PGMData {
    int xsize;
    int ysize;
    vector<unsigned char> image;
    FILE *Handle;
    bool loaded = 0;
    bool writen = 0;

public:
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

    image.reserve(xsize * ysize);

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

    fwrite(image.data(), sizeof(unsigned char), xsize * ysize, Handle);

    // Close file and return

    fclose(Handle);
    writen = 1;
    return;
}

    bool IsValidLocation(int x, int y){
    if (debug) {
        int opac = image[xsize * y + x];
        std::cout << "IVL: Opacity" << opac << std::endl;
        bool valid = opac != 0;
        if (valid)
            std::cout << "IVL:Valid Location\n";
        else
            std::cout << "IVL:Invalid Location\n";
        return valid;
    } else {

        return image[xsize * y + x] != 0;
    }
}

    bool AreValidLocations(vector<pair<int, int>> &points){
    for (auto i : points) {
        if (IsValidLocation(i.first, i.second)) {
        } else {
            if (debug)
                std::cout << "AVL:Invalid Location\n";
            return false;
        }
    }
    if (debug)
        std::cout << "AVL:Valid Location\n";
    return true;
}

    void DrawPoint(int x, int y, int opacity){
    size_t location = xsize * y + x;
    image[location] = opacity;
}

    void DrawPoints(vector<pair<int, int>> &points, int opacity){
    for (auto i : points) {
        DrawPoint(i.first, i.second, opacity);
    }
}

    void Print(){
    for (int y = 0; y < ysize; y++) {
        for (int x = 0; x < xsize; x++) {
            if ((image[y * xsize + x]) < 64) {
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








