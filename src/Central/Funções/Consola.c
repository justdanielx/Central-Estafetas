#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "../Central.h"

static char entrada[TAM_LINHA_CMD];
static size_t tamEntrada = 0;

static int leComando(char *buf, size_t tamanho) {
    char *fim;
    size_t len;
    ssize_t n;
    int descartar = 0;

    for (;;) {
        fim = memchr(entrada, '\n', tamEntrada);
        if (fim != NULL) {
            len = (size_t) (fim - entrada);
            if (!descartar && len < tamanho) {
                memcpy(buf, entrada, len);
                buf[len] = '\0';
            }
            tamEntrada -= len + 1;
            memmove(entrada, fim + 1, tamEntrada);
            return descartar ? -2 : 1;
        }
        if (tamEntrada == sizeof(entrada)) {
            tamEntrada = 0;
            descartar = 1;
        }
        n = read(STDIN_FILENO, entrada + tamEntrada, sizeof(entrada) - tamEntrada);
        if (n == -1)
            return -1;
        if (n == 0) {
            if (tamEntrada == 0 || descartar)
                return 0;
            len = tamEntrada < tamanho ? tamEntrada : tamanho - 1;
            memcpy(buf, entrada, len);
            buf[len] = '\0';
            tamEntrada = 0;
            return 1;
        }
        tamEntrada += (size_t) n;
    }
}

static int separaTokens(char *linha, char *tokens[], int max) {
    int n = 0;
    char *token = strtok(linha, " \t\r");

    while (token != NULL) {
        if (n == max)
            return max + 1;
        tokens[n++] = token;
        token = strtok(NULL, " \t\r");
    }
    return n;
}

static void cmdStats(const Central *central) {
    const Estatisticas *stats = &central->stats;

    printf("[STATS] Recebidos: %d | Atribuidos: %d | Concluidos: %d | Expirados: %d | Perdidos: %d\n",
           stats->recebidos, stats->atribuidos, stats->concluidos, stats->expirados, stats->perdidos);
    printf("[STATS] Valor total das entregas concluidas: %.2f EUR\n", stats->valorConcluido);
}

static int cmdPedido(Central *central, char *tokens[]) {
    float peso, distancia, valor;
    int r;

    if (strlen(tokens[1]) >= TAM_LOCAL || strlen(tokens[2]) >= TAM_LOCAL) {
        printf("[ERRO] Origem e destino tem no maximo %d caracteres\n", TAM_LOCAL - 1);
        return 0;
    }
    if (leRealPositivo(tokens[3], &peso) == -1) {
        printf("[ERRO] Peso invalido: '%s' (tem de ser um numero real positivo)\n", tokens[3]);
        return 0;
    }

    r = tarifa(central, tokens[1], tokens[2], peso, &distancia, &valor);
    if (r == -3) {
        printf("[ERRO] Nao foi possivel relancar o tarifador: a central vai encerrar\n");
        return 1;
    }
    if (r != 0) {
        printf("[ERRO] Nao foi possivel tarifar o pedido\n");
        return 0;
    }

    central->stats.recebidos++;
    printf("[PEDIDO] %s -> %s, %.2f kg: distancia %.1f km, valor %.2f EUR\n",
           tokens[1], tokens[2], peso, distancia, valor);
    return 0;
}

static void cmdNaoDisponivel(const char *comando) {
    printf("[INFO] Comando '%s' ainda nao implementado\n", comando);
}

static int processaComando(Central *central, char *linha) {
    char *tokens[MAX_TOKENS];
    int n, valor;

    n = separaTokens(linha, tokens, MAX_TOKENS);
    if (n == 0)
        return 0;
    if (n > MAX_TOKENS - 1) {
        printf("[ERRO] Demasiados parametros\n");
        return 0;
    }

    if (strcmp(tokens[0], "encerra") == 0) {
        if (n != 1) {
            printf("[ERRO] Sintaxe: encerra\n");
            return 0;
        }
        printf("[CENTRAL] A encerrar a pedido do administrador\n");
        return 1;
    }
    if (strcmp(tokens[0], "stats") == 0) {
        if (n != 1)
            printf("[ERRO] Sintaxe: stats\n");
        else
            cmdStats(central);
        return 0;
    }
    if (strcmp(tokens[0], "pedido") == 0) {
        if (n != 4) {
            printf("[ERRO] Sintaxe: pedido <origem> <destino> <peso>\n");
            return 0;
        }
        return cmdPedido(central, tokens);
    }
    if (strcmp(tokens[0], "estafetas") == 0 || strcmp(tokens[0], "pedidos") == 0) {
        if (n != 1)
            printf("[ERRO] Sintaxe: %s\n", tokens[0]);
        else
            cmdNaoDisponivel(tokens[0]);
        return 0;
    }
    if (strcmp(tokens[0], "prazo") == 0) {
        if (n != 2 || leInteiroPositivo(tokens[1], &valor) == -1)
            printf("[ERRO] Sintaxe: prazo <segundos> (inteiro positivo)\n");
        else
            cmdNaoDisponivel(tokens[0]);
        return 0;
    }

    printf("[ERRO] Comando desconhecido: '%s'\n", tokens[0]);
    printf("       Comandos: estafetas | pedidos | stats | prazo <n> | encerra | pedido <origem> <destino> <peso>\n");
    return 0;
}

void executaConsola(Central *central) {
    char linha[TAM_LINHA_CMD];
    int r, sair = 0;

    while (!terminar && !sair) {
        printf("admin> ");
        r = leComando(linha, sizeof(linha));
        if (r == -1) {
            if (errno == EINTR)
                continue;
            perror("[ERRO] read");
            break;
        }
        if (r == 0) {
            printf("\n[CENTRAL] Fim da entrada de comandos\n");
            break;
        }
        if (r == -2) {
            printf("[ERRO] Comando demasiado longo\n");
            continue;
        }
        sair = processaComando(central, linha);
    }

    if (terminar)
        printf("\n[CENTRAL] SIGINT recebido: a encerrar\n");
}