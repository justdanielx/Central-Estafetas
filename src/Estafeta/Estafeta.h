#ifndef ESTAFETA_H
#define ESTAFETA_H

#include <sys/types.h>
#include "../Protocolo/Protocolo.h"

#define TAM_LINHA_CMD 256
#define TAM_CAMINHO 256
#define SEGUNDOS_ESPERA_REGISTO 5

typedef struct {
    char username[TAM_USERNAME];
    char central_pipe[TAM_CAMINHO];
    char pipe_para_central[TAM_NOME_PIPE];
    char pipe_para_estafeta[TAM_NOME_PIPE];
    int fd_escrita;
    int fd_leitura;
} ConfigEstafeta;

#endif