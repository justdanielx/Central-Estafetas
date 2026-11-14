#ifndef CENTRAL_H
#define CENTRAL_H

#include <sys/types.h>
#include <signal.h>

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
#define MAX_TOKENS 6

typedef struct {
    int maxEstafetas;
    int prazoAceita;
    char bindir[TAM_CAMINHO];
    char centralPipe[TAM_CAMINHO];
} ConfigCentral;

typedef struct {
    pid_t pid;
    int fdEscrita;
    int fdLeitura;
} Tarifador;

typedef struct {
    int recebidos;
    int atribuidos;
    int concluidos;
    int expirados;
    int perdidos;
    float valorConcluido;
} Estatisticas;

typedef struct {
    ConfigCentral config;
    Tarifador tarifador;
    Estatisticas stats;
} Central;

extern volatile sig_atomic_t terminar;
extern volatile sig_atomic_t alarme;

//Sinais
int instalaSinais(void);

//Configuração
int leInteiroPositivo(const char *texto, int *valor);
int leRealPositivo(const char *texto, float *valor);
int obtemConfig(int argc, char *argv[], ConfigCentral *config);

//Instância
int verificaInstanciaUnica(void);
void libertaInstancia(void);

//Tarifador
int lancaTarifador(Central *central);
int testaTarifador(Central *central);
void terminaTarifador(Central *central, int esperaLimitada);
int tarifa(Central *central, const char *origem, const char *destino, float peso, float *distancia, float *valor);

//Consola
void executaConsola(Central *central);

//Central
int iniciaCentral(Central *central, int argc, char *argv[]);
void encerraCentral(Central *central);

#endif