#include "PPMFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

RGB ** createImage(int width, int height);
void freeImage(RGB ** image, int height);
void saveImage(char * name, int width, int height, int max, RGB ** image);

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

RGB ** createImage(int width, int height) {
    RGB *image = (RGB*) malloc(sizeof(struct RGB) * height);

    if(!image) return NULL;

    for(int i = 0; i < height; ++i) {
        image[i] = (RGB) malloc(sizeof(strRGB) * width);
        if(!image[i]) {
            freeImage(image, i);
            return NULL;
        }
    }

    return image;
}

void freeImage(RGB **image, int height) {
    for(int i = 0; i < height; ++i) {
        free(image[i]);
    }
    free(image);
}

void saveImage(char *name, int width, int height, int max, RGB **image){
    FILE * f = fopen(name, "w");
    if (!f) return;

    fprintf(f, "P3\n%d %d\n%d\n", width, height, max);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(f, "%d %d %d\n", image[i][j].red, image[i][j].green, image[i][j].blue);
        }
    }
    fclose(f);
}

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

    fscanf(f, "%d %d", &pf->width, &pf->height);
    fscanf(f, "%d", pf->maxColor);

    pf->image = (RGB*) malloc(pf->height * sizeof(struct RGB));
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
    if (!pf) return;

    RGB **bwImage = createImage(pf->width, pf->height);
    if (!bwImage) return;

    for(int i = 0; i < pf->height; ++i) {
        for(int j = 0; j < pf->width; ++j) {
            unsigned int r = pf->image[i][j].red;
            unsigned int g = pf->image[i][j].green;
            unsigned int b = pf->image[i][j].blue;

            int bw = (unsigned int) round(((r + g + b) / 3.0) / pf->maxColor);

            pf->image[i][j].red = bw;
            pf->image[i][j].green = bw;
            pf->image[i][j].blue = bw;
        }
    }

    int prefixlength = strlen("bw_");
    int filenamelength = strlen(pf->lengthName);
    char *newName = (char*) malloc(sizeof(char) * (prefixlength + filenamelength + 1));

    if (!newName) {
        freeImage(bwImage, pf->height);
        return;
    }

    strcpy(newName, "bw_");
    strcat(newName, pf->filename);

    saveImage(newName, pf->width, pf->height, 1, bwImage);
    free(newName);
    freeImage(bwImage, pf->height);
}

void ppmFile_grayscale(PPMFile pf) {
    if (!pf) return;

    RGB **gsImage = createImage(pf->width, pf->height);
    if (!gsImage) return;

    for(int i = 0; i < pf->height; ++i) {
        for(int j = 0; j < pf->width; ++j) {
            unsigned int r = pf->image[i][j].red;
            unsigned int g = pf->image[i][j].green;
            unsigned int b = pf->image[i][j].blue;

            int gs = (unsigned int) round((r + g + b) / 3.0);

            gsImage[i][j].red = gs;
            gsImage[i][j].green = gs;
            gsImage[i][j].blue = gs;
        }
    }

    int prefixlength = strlen("gs_");
    int filenamelength = strlen(pf->lengthName);
    char *newName = (char*) malloc(sizeof(char) * (prefixlength + filenamelength + 1));

    if (!newName) {
        freeImage(gsImage, pf->height);
        return;
    }

    strcpy(newName, "gs_");
    strcat(newName, pf->filename);

    saveImage(newName, pf->width, pf->height, pf->maxColor, gsImage);
    free(newName);
    freeImage(gsImage, pf->height);
}

void ppmFile_compress(PPMFile pf) {
    if (!pf) return;

    RGB **cmpImage = createImage(pf->width, pf->height);
    if (!cmpImage) return;

    for(int i = 0; i < pf->height; ++i) {
        for(int j = 0; j < pf->width; ++j) {
            unsigned int r = pf->image[i][j].red;
            unsigned int g = pf->image[i][j].green;
            unsigned int b = pf->image[i][j].blue;

            r = (unsigned int) round(r / 255.0);
            g = (unsigned int) round(g / 255.0);
            b = (unsigned int) round(b / 255.0);

            cmpImage[i][j].red = r;
            cmpImage[i][j].green = g;
            cmpImage[i][j].blue = b;
        }
    }

    int prefixlength = strlen("cmp_");
    char *newName = (char*) malloc(sizeof(char) * (prefixlength + pf->lengthName + 1));

    if(!newName) {
        freeImage(cmpImage, pf->height);
        return;
    }

    strcpy(newName, "cmp_");
    strcat(newName, pf->filename);

    saveImage(newName, pf->width, pf->height, pf->maxColor, cmpImage);
    free(newName);
    freeImage(cmpImage, pf->height);
}

void ppmFile_dithering(PPMFile pf) {
    if (!pf) return;

    int height = pf->height - (pf->height % 2);
    int width = pf->width - (pf->width % 2);

    RGB **dthImage = createImage(pf->width, pf->height);
    if (!dthImage) return;

    int mask_100[2][2] = {{1, 1}, {1, 1}};
    int mask_75[2][2] = {{1, 1}, {0, 1}};
    int mask_50[2][2] = {{1, 0}, {0, 1}};
    int mask_25[2][2] = {{1, 0}, {0, 0}};
    int mask_0[2][2] = {{0, 0}, {0, 0}};

    for(int i = 0; i < pf->height; ++i) {
        for(int j = 0; j < pf->width; ++j) {
            // Promedios de cada componente en el bloque 2x2
            float redAvg = 0, greenAvg = 0, blueAvg = 0;
            for (int x = 0; x < 2; ++x) {
                for (int y = 0; y < 2; ++y) {
                    redAvg += pf->image[i + x][j + y].red;
                    greenAvg += pf->image[i + x][j + y].green;
                    blueAvg += pf->image[i + x][j + y].blue;
                }
            }
            redAvg /= 4;
            greenAvg /= 4;
            blueAvg /= 4;

            // Seleccionar máscaras según promedios
            int (*redMask)[2] = (redAvg > 0.875 * 255) ? mask_100 :
                                (redAvg > 0.625 * 255) ? mask_75 :
                                (redAvg > 0.375 * 255) ? mask_50 :
                                (redAvg > 0.125 * 255) ? mask_25 : mask_0;

            int (*greenMask)[2] = (greenAvg > 0.875 * 255) ? mask_100 :
                                  (greenAvg > 0.625 * 255) ? mask_75 :
                                  (greenAvg > 0.375 * 255) ? mask_50 :
                                  (greenAvg > 0.125 * 255) ? mask_25 : mask_0;

            int (*blueMask)[2] = (blueAvg > 0.875 * 255) ? mask_100 :
                                 (blueAvg > 0.625 * 255) ? mask_75 :
                                 (blueAvg > 0.375 * 255) ? mask_50 :
                                 (blueAvg > 0.125 * 255) ? mask_25 : mask_0;

            // Asignar los valores ditherizados a los 4 píxeles en dthImage
            for (int x = 0; x < 2; ++x) {
                for (int y = 0; y < 2; ++y) {
                    dthImage[i + x][j + y].red = redMask[x][y] * pf->maxColor;
                    dthImage[i + x][j + y].green = greenMask[x][y] * pf->maxColor;
                    dthImage[i + x][j + y].blue = blueMask[x][y] * pf->maxColor;
                }
            }
        }
    }

    int prefixlength = strlen("dth_");
    char *newName = (char*) malloc(sizeof(char) * (prefixlength + pf->lengthName + 1));

    if(!newName) {
        freeImage(dthImage, pf->height);
        return;
    }

    strcpy(newName, "dth_");
    strcat(newName, pf->filename);

    saveImage(newName, pf->width, pf->height, pf->maxColor, dthImage);
    free(newName);
    freeImage(dthImage, pf->height);
}

void ppmFile_destroy(PPMFile pf) {
    if(!image) return;
    freeImage(pf->image, pf->height);
    free(pf->filename);
    free(pf);
}