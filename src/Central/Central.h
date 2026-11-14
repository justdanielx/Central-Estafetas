#ifndef CENTRAL_H
#define CENTRAL_H

#include <sys/types.h>
#include "../Protocolo/Protocolo.h"

#define DEF_MAXESTAFETAS 10
#define DEF_PRAZOACEITA 30
#define DEF_BINDIR "."

#define FICHEIRO_INSTANCIA "/tmp/central_estafetas.pid"
#define NOME_TARIFADOR "tarifador"
#define TESTE_TARIFADOR "restaurante_ze isec 2.4\n"
#define SEGUNDOS_ESPERA_TARIFADOR 3

#define TAM_LINHA_CMD 256
#define TAM_LINHA_TARIFADOR 128
#define TAM_CAMINHO 512

typedef struct {
    int max_estafetas;
    int prazo_aceita;
    char bindir[TAM_CAMINHO];
    char central_pipe[TAM_CAMINHO];
} ConfigCentral;

typedef struct {
    pid_t pid;
    int fd_escrita;
    int fd_leitura;
} Tarifador;

typedef struct {
    int recebidos;
    int atribuidos;
    int concluidos;
    int expirados;
    int perdidos;
    float valor_concluido;
} Estatisticas;

typedef struct {
    ConfigCentral config;
    Tarifador tarifador;
    Estatisticas stats;
} Central;

#endif