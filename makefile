CC = gcc
CFLAGS = -Wall -Wextra
DEPS = src/Central/Central.h src/Protocolo/Protocolo.h
OBJ = obj/main.o obj/Central.o obj/Sinais.o obj/Configuração.o obj/Instância.o obj/Tarifador.o obj/Consola.o

all: central

central: $(OBJ)
	$(CC) $(CFLAGS) -o central $(OBJ)

obj/main.o: src/Central/main.c $(DEPS)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ src/Central/main.c

obj/Central.o: src/Central/Central.c $(DEPS)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ src/Central/Central.c

obj/Sinais.o: src/Central/Funções/Sinais.c $(DEPS)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ src/Central/Funções/Sinais.c

obj/Configuração.o: src/Central/Funções/Configuração.c $(DEPS)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ src/Central/Funções/Configuração.c

obj/Instância.o: src/Central/Funções/Instância.c $(DEPS)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ src/Central/Funções/Instância.c

obj/Tarifador.o: src/Central/Funções/Tarifador.c $(DEPS)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ src/Central/Funções/Tarifador.c

obj/Consola.o: src/Central/Funções/Consola.c $(DEPS)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ src/Central/Funções/Consola.c

clean:
	rm -f central
	rm -rf obj
