#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#include <sys/types.h>

//Constantes
#define TAM_TIPO 12
#define TAM_CORPO 50
#define TAM_LOCAL 16 //15 + \0
#define TAM_USERNAME 20
#define TAM_NOME_PIPE 64

#define MAX_ESTAFETAS_ABS 20
#define MAX_PEDIDOS_POR_ATRIBUIR 30

#define VAR_CENTRAL_PIPE "CENTRAL_PIPE"
#define VAR_MAXESTAFETAS "MAXESTAFETAS"
#define VAR_PRAZOACEITA "PRAZOACEITA"
#define VAR_BINDIR "BINDIR"
#define VAR_INTMIN "INTMIN"
#define VAR_INTMAX "INTMAX"

//Tipos de Mensagem
#define TIPO_PEDIDO "pedido"
#define TIPO_REGISTO "RegistoEst"
#define TIPO_RESP_REGISTO "RespRegisto"
#define TIPO_ANUNCIO "Anuncio"
#define TIPO_RETIRADO "Retirado"
#define TIPO_ACEITA "Aceita"
#define TIPO_RESP_ACEITA "RespAceita"
#define TIPO_ENTREGUE "Entregue"
#define TIPO_RESP_ENTREGUE "RespEntrega"
#define TIPO_PED_ESTADO "PedEstado"
#define TIPO_RESP_ESTADO "RespEstado"
#define TIPO_PED_GANHOS "PedGanhos"
#define TIPO_RESP_GANHOS "RespGanhos"
#define TIPO_SAI "Sai"
#define TIPO_ENCERRA "Encerra"

//Nomes dos Pipes dos Estafetas
#define PREFIXO_PIPE_EST "/tmp/central_estafetas_est_"
#define FMT_PIPE_EST_PARA_CENTRAL "/tmp/central_estafetas_est_%d_e2c"
#define FMT_PIPE_CENTRAL_PARA_EST "/tmp/central_estafetas_est_%d_c2e"

//Estados e Respostas
typedef enum {
    PEDIDO_POR_ATRIBUIR,
    PEDIDO_EM_CURSO,
    PEDIDO_CONCLUIDO,
    PEDIDO_EXPIRADO,
    PEDIDO_PERDIDO
} EstadoPedido;

typedef enum {
    ACEITA_ATRIBUIDO,
    ACEITA_JA_ATRIBUIDO,
    ACEITA_EXPIRADO,
    ACEITA_NAO_EXISTE,
    ACEITA_ESTAFETA_OCUPADO
} ResultadoAceita;

typedef enum {
    REGISTO_OK,
    REGISTO_NOME_REPETIDO,
    REGISTO_CHEIO,
    REGISTO_ERRO
} ResultadoRegisto;

//Estruturas das Mensagens
typedef struct {
    char tipo[TAM_TIPO];
} CabecalhoMsg;

typedef struct {
    char tipo[TAM_TIPO];
    char corpo[TAM_CORPO];
} MsgPedido;

typedef struct {
    char tipo[TAM_TIPO];
    char username[TAM_USERNAME];
    pid_t pid;
    char pipe_para_central[TAM_NOME_PIPE];
    char pipe_para_estafeta[TAM_NOME_PIPE];
} MsgRegisto;

typedef struct {
    char tipo[TAM_TIPO];
    ResultadoRegisto resultado;
} MsgRespRegisto;

typedef struct {
    char tipo[TAM_TIPO];
    int id;
    char origem[TAM_LOCAL];
    char destino[TAM_LOCAL];
    float peso;
    float distancia;
    float valor;
    int segundos_restantes;
} MsgAnuncio;

typedef struct {
    char tipo[TAM_TIPO];
    int id;
} MsgRetirado;

typedef struct {
    char tipo[TAM_TIPO];
    char username[TAM_USERNAME];
    int id;
} MsgAceita;

typedef struct {
    char tipo[TAM_TIPO];
    int id;
    ResultadoAceita resultado;
} MsgRespAceita;

typedef struct {
    char tipo[TAM_TIPO];
    char username[TAM_USERNAME];
} MsgEstafeta;

typedef struct {
    char tipo[TAM_TIPO];
    int sucesso;
    int id;
    float valor;
} MsgRespEntregue;

typedef struct {
    char tipo[TAM_TIPO];
    int ocupado;
    int id;
    char origem[TAM_LOCAL];
    char destino[TAM_LOCAL];
    float peso;
    float valor;
} MsgRespEstado;

typedef struct {
    char tipo[TAM_TIPO];
    int entregas;
    float total;
} MsgRespGanhos;

typedef struct {
    char tipo[TAM_TIPO];
} MsgSimples;

#endif
