CC = gcc
CFLAGS = -Wall -Wextra
DEPS = src/Central/Central.h src/Protocolo/Protocolo.h
OBJ = src/Central/main.o src/Central/Central.o src/Central/Funções/Sinais.o src/Central/Funções/Configuração.o src/Central/Funções/Instância.o src/Central/Funções/Tarifador.o src/Central/Funções/Consola.o

all: central

central: $(OBJ)
	$(CC) $(CFLAGS) -o central $(OBJ)

src/Central/main.o: src/Central/main.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ src/Central/main.c

src/Central/Central.o: src/Central/Central.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ src/Central/Central.c

src/Central/Funções/Sinais.o: src/Central/Funções/Sinais.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ src/Central/Funções/Sinais.c

src/Central/Funções/Configuração.o: src/Central/Funções/Configuração.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ src/Central/Funções/Configuração.c

src/Central/Funções/Instância.o: src/Central/Funções/Instância.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ src/Central/Funções/Instância.c

src/Central/Funções/Tarifador.o: src/Central/Funções/Tarifador.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ src/Central/Funções/Tarifador.c

src/Central/Funções/Consola.o: src/Central/Funções/Consola.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ src/Central/Funções/Consola.c

clean:
	rm -f central src/Central/*.o src/Central/Funções/*.o
