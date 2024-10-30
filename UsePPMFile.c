#include "PPMFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char mensaje = "---------------------------------";
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

    printf("%s", mensaje);

    ppmFile_blackAndWhite(pf);
    printf("%s", mensaje);

    ppmFile_grayscale(pf);
    printf("%s", mensaje);
    
    ppmFile_compress(pf);
    printf("%s", mensaje);
    
    ppmFile_dithering(pf);
    printf("%s", mensaje);

    free(filename);
    printf("%s", mensaje);
    
    ppmFile_destroy(pf);
    printf("%s", mensaje);

    return 0;
}