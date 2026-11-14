#ifndef ESTAFETA_H
#define ESTAFETA_H

#include <sys/types.h>

#include "../Protocolo/Protocolo.h"

#define TAM_LINHA_CMD 256
#define TAM_CAMINHO 256
#define SEGUNDOS_ESPERA_REGISTO 5

typedef struct {
    char username[TAM_USERNAME];
    char centralPipe[TAM_CAMINHO];
    char pipeParaCentral[TAM_NOME_PIPE];
    char pipeParaEstafeta[TAM_NOME_PIPE];
    int fdEscrita;
    int fdLeitura;
} ConfigEstafeta;

#endif