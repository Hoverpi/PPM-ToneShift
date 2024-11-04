#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "PPMFile.h"

typedef struct strRGB {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} RGB;

struct strPPMFile {
    char *filename;
    int lengthName;
    int width;
    int height;
    unsigned int maxColor;
    RGB **image;
} ;

RGB** createImage(int width, int height);
void freeImage(RGB **image, int height);
void saveImage(const char *filename, int width, int height, int maxColor, RGB **image);
// unsigned char clamp(unsigned char value, unsigned char min, unsigned char max);

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
            unsigned char r = pf->image[i][j].red;
            unsigned char g = pf->image[i][j].green;
            unsigned char b = pf->image[i][j].blue;

            unsigned char bw = (unsigned char) round(((r + g + b) / 3.0) / pf->maxColor);

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

    saveImage(newName, pf->width, pf->height, 1, bwImage);
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
            unsigned char r = pf->image[i][j].red;
            unsigned char g = pf->image[i][j].green;
            unsigned char b = pf->image[i][j].blue;

            unsigned char gs = (unsigned char) round((r + g + b) / 3.0);

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
            unsigned char r = pf->image[i][j].red;
            unsigned char g = pf->image[i][j].green;
            unsigned char b = pf->image[i][j].blue;

            r = (unsigned char) round(r / 255.0);
            g = (unsigned char) round(g / 255.0);
            b = (unsigned char) round(b / 255.0);

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

// void ppmFile_dithering(PPMFile pf) {
//     if (!pf) return;

//     // Crear la imagen dithered
//     RGB **dithImage = createImage(pf->width, pf->height);
//     if (!dithImage) {
//         fprintf(stderr, "Error al crear la imagen dithered.\n");
//         return;
//     }

//     for (int i = 0; i < pf->height; ++i) {
//         for (int j = 0; j < pf->width; ++j) {
//             dithImage[i][j].red = pf->image[i][j].red;
//             dithImage[i][j].green = pf->image[i][j].green;
//             dithImage[i][j].blue = pf->image[i][j].blue;
//         }
//     }

//     // Aplicar el algoritmo de dithering de Floyd-Steinberg
//     for (int y = 0; y < pf->height; y++) {
//         for (int x = 0; x < pf->width; x++) {
//             unsigned char oldRed = dithImage[y][x].red;
//             unsigned char oldGreen = dithImage[y][x].green;
//             unsigned char oldBlue = dithImage[y][x].blue;

//             // Cuantización a 0 o maxColor
//             unsigned char newRed = (oldRed > pf->maxColor / 2) ? pf->maxColor : 0;
//             unsigned char newGreen = (oldGreen > pf->maxColor / 2) ? pf->maxColor : 0;
//             unsigned char newBlue = (oldBlue > pf->maxColor / 2) ? pf->maxColor : 0;

//             dithImage[y][x].red = newRed;
//             dithImage[y][x].green = newGreen;
//             dithImage[y][x].blue = newBlue;

//             // Calcular errores
//             unsigned char redError = oldRed - newRed;
//             unsigned char greenError = oldGreen - newGreen;
//             unsigned char blueError = oldBlue - newBlue;

//             // Distribuir el error a los vecinos si están dentro de los límites
//             if (x + 1 < pf->width) {
//                 dithImage[y][x + 1].red = (unsigned char) clamp(dithImage[y][x + 1].red + redError * 7 / 16.0, 0, pf->maxColor);
//                 dithImage[y][x + 1].green = (unsigned char) clamp(dithImage[y][x + 1].green + greenError * 7 / 16.0, 0, pf->maxColor);
//                 dithImage[y][x + 1].blue = (unsigned char) clamp(dithImage[y][x + 1].blue + blueError * 7 / 16.0, 0, pf->maxColor);
//             }
//             if (y + 1 < pf->height) {
//                 if (x > 0) {
//                     dithImage[y + 1][x - 1].red = clamp(dithImage[y + 1][x - 1].red + redError * 3 / 16, 0, pf->maxColor);
//                     dithImage[y + 1][x - 1].green = clamp(dithImage[y + 1][x - 1].green + greenError * 3 / 16, 0, pf->maxColor);
//                     dithImage[y + 1][x - 1].blue = clamp(dithImage[y + 1][x - 1].blue + blueError * 3 / 16, 0, pf->maxColor);
//                 }
//                 dithImage[y + 1][x].red = clamp(dithImage[y + 1][x].red + redError * 5 / 16, 0, pf->maxColor);
//                 dithImage[y + 1][x].green = clamp(dithImage[y + 1][x].green + greenError * 5 / 16, 0, pf->maxColor);
//                 dithImage[y + 1][x].blue = clamp(dithImage[y + 1][x].blue + blueError * 5 / 16, 0, pf->maxColor);

//                 if (x + 1 < pf->width) {
//                     dithImage[y + 1][x + 1].red = clamp(dithImage[y + 1][x + 1].red + redError * 1 / 16, 0, pf->maxColor);
//                     dithImage[y + 1][x + 1].green = clamp(dithImage[y + 1][x + 1].green + greenError * 1 / 16, 0, pf->maxColor);
//                     dithImage[y + 1][x + 1].blue = clamp(dithImage[y + 1][x + 1].blue + blueError * 1 / 16, 0, pf->maxColor);
//                 }
//             }
//         }
//     }

//     // Guardar la imagen dithered
//     int prefixlength = strlen("dth_");
//     char *newName = (char*) malloc(sizeof(char) * (prefixlength + pf->lengthName + 1));
//     if (!newName) {
//         freeImage(dithImage, pf->height);
//         fprintf(stderr, "Error al asignar memoria para el nuevo nombre de archivo.\n");
//         return;
//     }

//     strcpy(newName, "dth_");
//     strcat(newName, pf->filename);

//     saveImage(newName, pf->width, pf->height, pf->maxColor, dithImage);

//     free(newName);
//     freeImage(dithImage, pf->height);
// }

void ppmFile_destroy(PPMFile pf) {
    if (pf) {
        freeImage(pf->image, pf->height);
        free(pf->filename);
        free(pf);
    }
}

// unsigned char clamp(unsigned char value, unsigned char min, unsigned char max) {
//     if (value < min) return min;
//     if (value > max) return max;
//     return value;
// }