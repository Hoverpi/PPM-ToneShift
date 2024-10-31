#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "PPMFile.h"

RGB **createImage(int width, int height) {
    RGB **image = (RGB **) malloc(sizeof(RGB *) * height);
    if (!image) {
        fprintf(stderr, "Error al asignar memoria para la imagen.\n");
        return NULL;
    }

    for (int i = 0; i < height; ++i) {
        image[i] = (RGB *) malloc(sizeof(RGB) * width);
        if (!image[i]) {
            freeImage(image, i);
            fprintf(stderr, "Error al asignar memoria para la fila %d de la imagen.\n", i);
            return NULL;
        }
    }

    return image;
}

void freeImage(RGB **image, int height) {
    for (int i = 0; i < height; ++i) {
        free(image[i]);
    }
    free(image);
}

void saveImage(const char *filename, int width, int height, int maxColor, RGB **image) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Error al abrir el archivo %s para guardar la imagen.\n", filename);
        return;
    }

    fprintf(f, "P3\n%d %d\n%d\n", width, height, maxColor);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(f, "%d %d %d\n", image[i][j].red, image[i][j].green, image[i][j].blue);
        }
    }
    fclose(f);
    printf("Imagen guardada: %s\n", filename);
}

PPMFile ppmFile_create(const char *name) {
    FILE *f = fopen(name, "r");
    if (!f) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    PPMFile pf = (PPMFile) malloc(sizeof(struct strPPMFile));
    if (!pf) {
        fclose(f);
        fprintf(stderr, "Error al asignar memoria para la estructura PPMFile.\n");
        return NULL;
    }

    pf->filename = (char *) malloc(sizeof(char) * (strlen(name) + 1));
    if (!pf->filename) {
        free(pf);
        fclose(f);
        fprintf(stderr, "Error al asignar memoria para el nombre del archivo.\n");
        return NULL;
    }

    strcpy(pf->filename, name);
    pf->lengthName = strlen(pf->filename);

    char format[3];
    if (!fgets(format, sizeof(format), f)) {
        free(pf->filename);
        free(pf);
        fclose(f);
        fprintf(stderr, "Error al leer el formato de la imagen.\n");
        return NULL;
    }

    if (fscanf(f, "%d %d", &pf->width, &pf->height) != 2 || fscanf(f, "%d", &pf->maxColor) != 1) {
        free(pf->filename);
        free(pf);
        fclose(f);
        fprintf(stderr, "Error al leer dimensiones o valor máximo de color.\n");
        return NULL;
    }

    pf->image = createImage(pf->width, pf->height);
    if (!pf->image) {
        free(pf->filename);
        free(pf);
        fclose(f);
        fprintf(stderr, "Error al crear la imagen.\n");
        return NULL;
    }

    for (int i = 0; i < pf->height; ++i) {
        for (int j = 0; j < pf->width; ++j) {
            if (fscanf(f, "%hhu %hhu %hhu", &pf->image[i][j].red, &pf->image[i][j].green, &pf->image[i][j].blue) != 3) {
                freeImage(pf->image, pf->height);
                free(pf->filename);
                free(pf);
                fclose(f);
                fprintf(stderr, "Error al leer el color de la imagen en la posición (%d, %d).\n", i, j);
                return NULL;
            }
        }
    }

    fclose(f);
    printf("Archivo PPM creado: %s\n", pf->filename);
    return pf;
}

void ppmFile_blackAndWhite(PPMFile pf) {
    if (!pf) return;

    RGB **bwImage = createImage(pf->width, pf->height);
    if (!bwImage) {
        fprintf(stderr, "Error al crear la imagen en blanco y negro.\n");
        return;
    }

    for (int i = 0; i < pf->height; ++i) {
        for (int j = 0; j < pf->width; ++j) {
            unsigned int r = pf->image[i][j].red;
            unsigned int g = pf->image[i][j].green;
            unsigned int b = pf->image[i][j].blue;

            unsigned int bw = (unsigned int) round(((r + g + b) / 3.0));

            bwImage[i][j].red = bw;
            bwImage[i][j].green = bw;
            bwImage[i][j].blue = bw;
        }
    }

    int prefixlength = strlen("bw_");
    char *newName = (char *) malloc(sizeof(char) * (prefixlength + pf->lengthName + 1));

    if (!newName) {
        freeImage(bwImage, pf->height);
        fprintf(stderr, "Error al asignar memoria para el nuevo nombre de archivo.\n");
        return;
    }

    strcpy(newName, "bw_");
    strcat(newName, pf->filename);

    saveImage(newName, pf->width, pf->height, 255, bwImage);
    free(newName);
    freeImage(bwImage, pf->height);
}

void ppmFile_grayscale(PPMFile pf) {
    if (!pf) return;

    RGB **gsImage = createImage(pf->width, pf->height);
    if (!gsImage) {
        fprintf(stderr, "Error al crear la imagen en escala de grises.\n");
        return;
    }

    for (int i = 0; i < pf->height; ++i) {
        for (int j = 0; j < pf->width; ++j) {
            unsigned int r = pf->image[i][j].red;
            unsigned int g = pf->image[i][j].green;
            unsigned int b = pf->image[i][j].blue;

            unsigned int gs = (unsigned int) round((r + g + b) / 3.0);

            gsImage[i][j].red = gs;
            gsImage[i][j].green = gs;
            gsImage[i][j].blue = gs;
        }
    }

    int prefixlength = strlen("gs_");
    char *newName = (char *) malloc(sizeof(char) * (prefixlength + pf->lengthName + 1));

    if (!newName) {
        freeImage(gsImage, pf->height);
        fprintf(stderr, "Error al asignar memoria para el nuevo nombre de archivo.\n");
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
    if (!cmpImage) {
        fprintf(stderr, "Error al crear la imagen comprimida.\n");
        return;
    }

    for (int i = 0; i < pf->height; ++i) {
        for (int j = 0; j < pf->width; ++j) {
            unsigned int r = pf->image[i][j].red;
            unsigned int g = pf->image[i][j].green;
            unsigned int b = pf->image[i][j].blue;

            r = (unsigned int) round(r / 255.0);
            g = (unsigned int) round(g / 255.0);
            b = (unsigned int) round(b / 255.0);

            cmpImage[i][j].red = r * 255;
            cmpImage[i][j].green = g * 255;
            cmpImage[i][j].blue = b * 255;
        }
    }

    int prefixlength = strlen("cmp_");
    char *newName = (char *) malloc(sizeof(char) * (prefixlength + pf->lengthName + 1));

    if (!newName) {
        freeImage(cmpImage, pf->height);
        fprintf(stderr, "Error al asignar memoria para el nuevo nombre de archivo.\n");
        return;
    }

    strcpy(newName, "cmp_");
    strcat(newName, pf->filename);

    saveImage(newName, pf->width, pf->height, 255, cmpImage);
    free(newName);
    freeImage(cmpImage, pf->height);
}

void ppmFile_dithering(PPMFile pf) {
    if (!pf) return;

    // Asegurarse de que las dimensiones son pares
    int height = pf->height - (pf->height % 2);
    int width = pf->width - (pf->width % 2);

    // Crear la imagen dithered
    RGB **dithImage = createImage(width, height);
    if (!dithImage) {
        fprintf(stderr, "Error al crear la imagen dithered.\n");
        return;
    }

    for (int i = 0; i < height; i += 2) {
        for (int j = 0; j < width; j += 2) {
            // Obtener los colores de los cuatro píxeles
            unsigned int r1 = pf->image[i][j].red;
            unsigned int g1 = pf->image[i][j].green;
            unsigned int b1 = pf->image[i][j].blue;

            unsigned int r2 = pf->image[i][j + 1].red;
            unsigned int g2 = pf->image[i][j + 1].green;
            unsigned int b2 = pf->image[i][j + 1].blue;

            unsigned int r3 = pf->image[i + 1][j].red;
            unsigned int g3 = pf->image[i + 1][j].green;
            unsigned int b3 = pf->image[i + 1][j].blue;

            unsigned int r4 = pf->image[i + 1][j + 1].red;
            unsigned int g4 = pf->image[i + 1][j + 1].green;
            unsigned int b4 = pf->image[i + 1][j + 1].blue;

            // Promediar los colores
            unsigned int rAvg = (r1 + r2 + r3 + r4) / 4;
            unsigned int gAvg = (g1 + g2 + g3 + g4) / 4;
            unsigned int bAvg = (b1 + b2 + b3 + b4) / 4;

            dithImage[i][j].red = rAvg;
            dithImage[i][j].green = gAvg;
            dithImage[i][j].blue = bAvg;

            dithImage[i][j + 1].red = rAvg;
            dithImage[i][j + 1].green = gAvg;
            dithImage[i][j + 1].blue = gAvg;

            dithImage[i + 1][j].red = rAvg;
            dithImage[i + 1][j].green = gAvg;
            dithImage[i + 1][j].blue = gAvg;

            dithImage[i + 1][j + 1].red = rAvg;
            dithImage[i + 1][j + 1].green = gAvg;
            dithImage[i + 1][j + 1].blue = gAvg;
        }
    }

    int prefixlength = strlen("dth_");
    char *newName = (char*) malloc(sizeof(char) * (prefixlength + pf->lengthName + 1));

    if (!newName) {
        freeImage(dithImage, height);
        fprintf(stderr, "Error al asignar memoria para el nuevo nombre de archivo.\n");
        return;
    }

    strcpy(newName, "dth_");
    strcat(newName, pf->filename);

    saveImage(newName, width, height, pf->maxColor, dithImage);
    free(newName);
    freeImage(dithImage, height);
}

void ppmFile_destroy(PPMFile pf) {
    if (pf) {
        freeImage(pf->image, pf->height);
        free(pf->filename);
        free(pf);
    }
}