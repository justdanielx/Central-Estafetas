#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../Central.h"

static int escreveTudo(int fd, const void *buf, size_t n) {
    const char *dados = buf;
    size_t enviados = 0;
    ssize_t r;

    while (enviados < n) {
        r = write(fd, dados + enviados, n - enviados);
        if (r == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        enviados += (size_t) r;
    }
    return 0;
}

static int leLinhaFd(int fd, char *buf, size_t tamanho) {
    size_t n = 0;
    ssize_t r;
    char caracter;

    while (n < tamanho - 1) {
        r = read(fd, &caracter, 1);
        if (r == -1) {
            if (errno == EINTR) {
                if (alarme)
                    return -2;
                continue;
            }
            return -1;
        }
        if (r == 0)
            return 0;
        if (caracter == '\n')
            break;
        buf[n++] = caracter;
    }
    buf[n] = '\0';
    return 1;
}

int lancaTarifador(Central *central) {
    char caminho[TAM_CAMINHO + sizeof(NOME_TARIFADOR) + 1];
    int paraTarifador[2], deTarifador[2];
    struct sigaction sa;
    pid_t pid;

    snprintf(caminho, sizeof(caminho), "%s/%s", central->config.bindir, NOME_TARIFADOR);
    if (access(caminho, X_OK) == -1) {
        fprintf(stderr, "[ERRO] Tarifador nao encontrado ou nao executavel: %s (%s)\n",
                caminho, strerror(errno));
        return -1;
    }

    if (pipe(paraTarifador) == -1) {
        perror("[ERRO] pipe");
        return -1;
    }
    if (pipe(deTarifador) == -1) {
        perror("[ERRO] pipe");
        close(paraTarifador[0]);
        close(paraTarifador[1]);
        return -1;
    }

    pid = fork();
    if (pid == -1) {
        perror("[ERRO] fork");
        close(paraTarifador[0]);
        close(paraTarifador[1]);
        close(deTarifador[0]);
        close(deTarifador[1]);
        return -1;
    }

    if (pid == 0) {
        memset(&sa, 0, sizeof(sa));
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = SIG_IGN;
        sigaction(SIGINT, &sa, NULL);
        sa.sa_handler = SIG_DFL;
        sigaction(SIGPIPE, &sa, NULL);

        close(STDIN_FILENO);
        if (dup(paraTarifador[0]) != STDIN_FILENO)
            _exit(127);
        close(STDOUT_FILENO);
        if (dup(deTarifador[1]) != STDOUT_FILENO)
            _exit(127);
        close(paraTarifador[0]);
        close(paraTarifador[1]);
        close(deTarifador[0]);
        close(deTarifador[1]);

        execl(caminho, NOME_TARIFADOR, (char *) NULL);
        perror("[ERRO] execl tarifador");
        _exit(127);
    }

    close(paraTarifador[0]);
    close(deTarifador[1]);
    central->tarifador.pid = pid;
    central->tarifador.fdEscrita = paraTarifador[1];
    central->tarifador.fdLeitura = deTarifador[0];
    return 0;
}

int testaTarifador(Central *central) {
    char resposta[TAM_LINHA_TARIFADOR];
    int r;

    alarme = 0;
    alarm(SEGUNDOS_ESPERA_TARIFADOR);
    r = escreveTudo(central->tarifador.fdEscrita, TESTE_TARIFADOR, strlen(TESTE_TARIFADOR));
    if (r == 0)
        r = leLinhaFd(central->tarifador.fdLeitura, resposta, sizeof(resposta));
    else
        r = -1;
    alarm(0);

    if (r != 1) {
        fprintf(stderr, "[ERRO] O tarifador nao arrancou ou nao respondeu em %d s\n", SEGUNDOS_ESPERA_TARIFADOR);
        return -1;
    }
    printf("[CENTRAL] Tarifador lancado (PID %d) e a responder\n", (int) central->tarifador.pid);
    return 0;
}

void terminaTarifador(Central *central, int esperaLimitada) {
    int estado;
    pid_t r;

    if (central->tarifador.pid <= 0)
        return;

    if (central->tarifador.fdEscrita != -1) {
        close(central->tarifador.fdEscrita);
        central->tarifador.fdEscrita = -1;
    }

    if (esperaLimitada) {
        alarme = 0;
        alarm(SEGUNDOS_ESPERA_TARIFADOR);
        do {
            r = waitpid(central->tarifador.pid, &estado, 0);
        } while (r == -1 && errno == EINTR && !alarme);
        alarm(0);
        if (r == -1 && alarme) {
            printf("[CENTRAL] Tarifador nao terminou a tempo: a enviar SIGTERM\n");
            kill(central->tarifador.pid, SIGTERM);
            do {
                r = waitpid(central->tarifador.pid, &estado, 0);
            } while (r == -1 && errno == EINTR);
        }
    } else {
        kill(central->tarifador.pid, SIGTERM);
        do {
            r = waitpid(central->tarifador.pid, &estado, 0);
        } while (r == -1 && errno == EINTR);
    }

    if (r == central->tarifador.pid) {
        if (WIFEXITED(estado))
            printf("[CENTRAL] Tarifador terminou (codigo %d)\n", WEXITSTATUS(estado));
        else if (WIFSIGNALED(estado))
            printf("[CENTRAL] Tarifador terminou pelo sinal %d\n", WTERMSIG(estado));
    }

    if (central->tarifador.fdLeitura != -1) {
        close(central->tarifador.fdLeitura);
        central->tarifador.fdLeitura = -1;
    }
    central->tarifador.pid = -1;
}

static int perguntaTarifador(Central *central, const char *origem, const char *destino,
                              float peso, float *distancia, float *valor) {
    char linha[TAM_LINHA_TARIFADOR];
    char resposta[TAM_LINHA_TARIFADOR];
    int n;

    if (central->tarifador.pid <= 0)
        return -1;
    n = snprintf(linha, sizeof(linha), "%s %s %.2f\n", origem, destino, peso);
    if (n < 0 || (size_t) n >= sizeof(linha))
        return -2;

    if (escreveTudo(central->tarifador.fdEscrita, linha, (size_t) n) == -1)
        return -1;
    if (leLinhaFd(central->tarifador.fdLeitura, resposta, sizeof(resposta)) != 1)
        return -1;
    if (sscanf(resposta, "%f %f", distancia, valor) != 2) {
        fprintf(stderr, "[ERRO] Resposta inesperada do tarifador: '%s'\n", resposta);
        return -2;
    }
    return 0;
}

int tarifa(Central *central, const char *origem, const char *destino,
                  float peso, float *distancia, float *valor) {
    int r;

    r = perguntaTarifador(central, origem, destino, peso, distancia, valor);
    if (r != -1)
        return r;

    fprintf(stderr, "[ERRO] Comunicacao com o tarifador falhou: a relancar\n");
    terminaTarifador(central, 0);
    if (lancaTarifador(central) == -1 || testaTarifador(central) == -1)
        return -3;
    return perguntaTarifador(central, origem, destino, peso, distancia, valor);
}