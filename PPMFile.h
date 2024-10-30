#ifndef PPMFILE_H_
#define PPMFILE_H_

typedef struct strPPMFile * PPMFile; 

PPMFile ppmFile_create(char * name);
void ppmFile_blackAndWhite(PPMFile);
void ppmFile_grayscale(PPMFile);
void ppmFile_compress(PPMFile);
void ppmFile_dithering(PPMFile);
void ppmFile_destroy(PPMFile);

#endif