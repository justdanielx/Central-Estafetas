#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../Central.h"

int leInteiroPositivo(const char *texto, int *valor) {
    char *fim;
    long n;

    if (texto == NULL || *texto == '\0')
        return -1;
    errno = 0;
    n = strtol(texto, &fim, 10);
    if (errno != 0 || *fim != '\0' || n <= 0 || n > 1000000)
        return -1;
    *valor = (int) n;
    return 0;
}

int leRealPositivo(const char *texto, float *valor) {
    char *fim;
    float n;

    if (texto == NULL || *texto == '\0')
        return -1;
    errno = 0;
    n = strtof(texto, &fim);
    if (errno != 0 || *fim != '\0' || !(n > 0.0f))
        return -1;
    *valor = n;
    return 0;
}

static int copiaTexto(char *destino, size_t tamanho, const char *origem) {
    if (strlen(origem) >= tamanho)
        return -1;
    strcpy(destino, origem);
    return 0;
}

static void mostraSintaxe(const char *nome) {
    fprintf(stderr, "Sintaxe: %s [-p <segundos>] [-m <numero>]\n", nome);
}

int obtemConfig(int argc, char *argv[], ConfigCentral *config) {
    char *valor;
    int i;

    config->maxEstafetas = DEF_MAXESTAFETAS;
    config->prazoAceita = DEF_PRAZOACEITA;
    strcpy(config->bindir, DEF_BINDIR);
    config->centralPipe[0] = '\0';

    valor = getenv(VAR_MAXESTAFETAS);
    if (valor != NULL && leInteiroPositivo(valor, &config->maxEstafetas) == -1) {
        fprintf(stderr, "[ERRO] Variavel %s invalida: '%s'\n", VAR_MAXESTAFETAS, valor);
        return -1;
    }

    valor = getenv(VAR_PRAZOACEITA);
    if (valor != NULL && leInteiroPositivo(valor, &config->prazoAceita) == -1) {
        fprintf(stderr, "[ERRO] Variavel %s invalida: '%s'\n", VAR_PRAZOACEITA, valor);
        return -1;
    }

    valor = getenv(VAR_BINDIR);
    if (valor != NULL && copiaTexto(config->bindir, sizeof(config->bindir), valor) == -1) {
        fprintf(stderr, "[ERRO] Variavel %s demasiado longa\n", VAR_BINDIR);
        return -1;
    }

    valor = getenv(VAR_CENTRAL_PIPE);
    if (valor != NULL && copiaTexto(config->centralPipe, sizeof(config->centralPipe), valor) == -1) {
        fprintf(stderr, "[ERRO] Variavel %s demasiado longa\n", VAR_CENTRAL_PIPE);
        return -1;
    }

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            if (leInteiroPositivo(argv[++i], &config->prazoAceita) == -1) {
                fprintf(stderr, "[ERRO] Valor invalido para -p: '%s'\n", argv[i]);
                return -1;
            }
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            if (leInteiroPositivo(argv[++i], &config->maxEstafetas) == -1) {
                fprintf(stderr, "[ERRO] Valor invalido para -m: '%s'\n", argv[i]);
                return -1;
            }
        } else {
            fprintf(stderr, "[ERRO] Argumento invalido: '%s'\n", argv[i]);
            mostraSintaxe(argv[0]);
            return -1;
        }
    }

    if (config->maxEstafetas > MAX_ESTAFETAS_ABS) {
        fprintf(stderr, "[ERRO] O maximo de estafetas nao pode exceder %d\n", MAX_ESTAFETAS_ABS);
        return -1;
    }

    return 0;
}