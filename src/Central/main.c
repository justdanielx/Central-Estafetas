#include <stdlib.h>

#include "Central.h"

int main(int argc, char *argv[]) {
    Central central;

    if (iniciaCentral(&central, argc, argv) == -1)
        return EXIT_FAILURE;

    executaConsola(&central);
    encerraCentral(&central);

    return EXIT_SUCCESS;
}