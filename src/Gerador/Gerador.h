#ifndef GERADOR_H
#define GERADOR_H

#include "../Protocolo/Protocolo.h"

#define DEF_INTMIN 2
#define DEF_INTMAX 10
#define TAM_CAMINHO 256

typedef struct {
    int idGerador;
    int intMin;
    int intMax;
    char centralPipe[TAM_CAMINHO];
} ConfigGerador;

#endif