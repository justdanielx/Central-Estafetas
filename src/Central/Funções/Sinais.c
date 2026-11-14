#include <string.h>
#include <signal.h>

#include "../Central.h"

volatile sig_atomic_t terminar = 0;
volatile sig_atomic_t alarme = 0;

static void trataSigint(int sinal) {
    (void) sinal;
    terminar = 1;
}

static void trataSigalrm(int sinal) {
    (void) sinal;
    alarme = 1;
}

int instalaSinais(void) {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sa.sa_handler = trataSigint;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        return -1;

    sa.sa_handler = trataSigalrm;
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        return -1;

    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) == -1)
        return -1;

    return 0;
}