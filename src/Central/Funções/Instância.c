#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include "../Central.h"

int verificaInstanciaUnica(void) {
    char texto[32];
    int tentativa, fd, n;
    pid_t pid;

    for (tentativa = 0; tentativa < 2; tentativa++) {
        fd = open(FICHEIRO_INSTANCIA, O_WRONLY | O_CREAT | O_EXCL, 0600);
        if (fd != -1) {
            n = snprintf(texto, sizeof(texto), "%d\n", (int) getpid());
            if (write(fd, texto, n) != n) {
                close(fd);
                unlink(FICHEIRO_INSTANCIA);
                return -1;
            }
            close(fd);
            return 0;
        }
        if (errno != EEXIST)
            return -1;

        fd = open(FICHEIRO_INSTANCIA, O_RDONLY);
        if (fd == -1)
            continue;
        n = read(fd, texto, sizeof(texto) - 1);
        close(fd);
        pid = 0;
        if (n > 0) {
            texto[n] = '\0';
            pid = (pid_t) atoi(texto);
        }
        if (pid > 0 && (kill(pid, 0) == 0 || errno == EPERM))
            return 1;
        unlink(FICHEIRO_INSTANCIA);
    }
    return -1;
}

void libertaInstancia(void) {
    unlink(FICHEIRO_INSTANCIA);
}