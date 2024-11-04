#ifndef PPMFILE_H
#define PPMFILE_H

typedef struct strPPMFile * PPMFile;

PPMFile ppmFile_create(const char *name);
void ppmFile_blackAndWhite(PPMFile pf);
void ppmFile_grayscale(PPMFile pf);
void ppmFile_compress(PPMFile pf);
void ppmFile_dithering(PPMFile pf);
void ppmFile_destroy(PPMFile pf);

#endif