#include <stdio.h>
#include <stdlib.h>
#include "PPMFile.h"

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} strRGB;

typedef struct strRGB * RGB;

typedef struct {
    char *filename;
    int lengthName;
    int width;
    int height;
    int maxColor;
    RGB *image;
} strPPMFile;

PPMFile ppmFile_create(char * name) {
    File * f = fopen(name, "r");
    if (!f) return NULL;

    PPMFile pf = (PPMFile) malloc(sizeof(struct strPPMFile));
    if (!pf) {
        fclose(f);
        return NULL;
    }
    pf->filename = (char*) malloc(sizeof(char) * (strlen(name) + 1));

    if(!pf->filename) {
        fclose(f);
        return NULL; 
    }

    strcpy(pf->filename, name);
    pf->lengthName = strlen(pf->filename);

    char format[3];
    fgets(format, sizeof(format), f); 

    fscanf(f, "%d %d", pf->width, pf->height);
    fscanf(f, "%d", pf->maxColor);
    pf->image = (RGB) malloc(pf->height * sizeof(struct strRGB));
    if (!pf->image) {
        free(pf->name);
        free(pf);
        fclose(f);
        return NULL;
    }

    for(int i = 0; i < pf->height; ++i) {
        pf->image[i] = (RGB) malloc(pf->width * sizeof(struct strRGB));
        if (!pf->image[i]) {
            for (int j = 0; j < i; ++j) free(pf->image[j]);
            free(pf->image);
            free(pf->name);
            free(pf);
            fclose(f);
            return NULL;
        }
    }

    for(int i = 0; i < pf->height; ++i) {
        for(int j = 0; j < pf->width; ++j) {
            fscanf(f, "hhu hhu hhu", pf->image[i][j]->red, pf->image[i][j]->green, pf->image[i][j]->blue);
        }
    }

    fclose(f);

    return pf;
}

void ppmFile_blackAndWhite(PPMFile pf) {
    
}
void ppmFile_grayscale(PPMFile pf);
void ppmFile_compress(PPMFile pf);
void ppmFile_dithering(PPMFile pf);
void ppmFile_destroy(PPMFile pf);