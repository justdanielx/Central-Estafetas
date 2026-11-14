#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "Central.h"

int iniciaCentral(Central *central, int argc, char *argv[]) {
    int r;

    setbuf(stdout, NULL);
    memset(central, 0, sizeof(*central));
    central->tarifador.pid = -1;
    central->tarifador.fdEscrita = -1;
    central->tarifador.fdLeitura = -1;

    if (instalaSinais() == -1) {
        perror("[ERRO] sigaction");
        return -1;
    }

    r = verificaInstanciaUnica();
    if (r == 1) {
        fprintf(stderr, "[ERRO] Ja existe uma central em execucao\n");
        return -1;
    }
    if (r == -1) {
        fprintf(stderr, "[ERRO] Nao foi possivel verificar a instancia unica (%s)\n", FICHEIRO_INSTANCIA);
        return -1;
    }

    if (obtemConfig(argc, argv, &central->config) == -1) {
        libertaInstancia();
        return -1;
    }

    if (lancaTarifador(central) == -1) {
        libertaInstancia();
        return -1;
    }
    if (testaTarifador(central) == -1) {
        terminaTarifador(central, 1);
        libertaInstancia();
        return -1;
    }

    printf("[CENTRAL] Em execucao (PID %d) | max. estafetas: %d | prazo: %d s | BINDIR: %s\n",
           (int) getpid(), central->config.maxEstafetas, central->config.prazoAceita,
           central->config.bindir);
    if (central->config.centralPipe[0] == '\0')
        printf("[AVISO] %s nao definida\n", VAR_CENTRAL_PIPE);
    else
        printf("[CENTRAL] %s: %s\n", VAR_CENTRAL_PIPE, central->config.centralPipe);

    return 0;
}

void encerraCentral(Central *central) {
    terminaTarifador(central, 1);
    libertaInstancia();
    printf("[CENTRAL] Central terminada\n");
}