#include "PPMFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char temp[30];
    printf("Ingrese el nombre del archivo: ");
    scanf("%s", temp);

    char *filename = (char*) malloc(sizeof(char) * (strlen(temp) + 1));
    if(!filename) {
        perror("Error al asignar memoria");
        return 1;
    }

    strcpy(filename, temp);

    PPMFile pf = ppmFile_create(filename);
    if (!pf) {
        fprintf(stderr, "No se pudo abrir o procesar el archivo.\n");
        free(filename);
        return 1;
    }

    ppmFile_blackAndWhite(pf);
    ppmFile_grayscale(pf);
    ppmFile_compress(pf);
    ppmFile_dithering(pf);

    free(filename);
    ppmFile_destroy(pf);

    return 0;
}