#ifndef PPMFILE_H
#define PPMFILE_H

typedef struct strRGB {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} RGB;

typedef struct strPPMFile {
    char *filename;
    int lengthName;
    int width;
    int height;
    int maxColor;
    RGB **image;
} *PPMFile;

RGB** createImage(int width, int height);
void freeImage(RGB **image, int height);
void saveImage(const char *filename, int width, int height, int maxColor, RGB **image);

PPMFile ppmFile_create(const char *name);
void ppmFile_blackAndWhite(PPMFile pf);
void ppmFile_grayscale(PPMFile pf);
void ppmFile_compress(PPMFile pf);
void ppmFile_dithering(PPMFile pf);
void ppmFile_destroy(PPMFile pf);

#endif