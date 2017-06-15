# Makefile Jorge
GROUP=G-2313-06-BOT

# Ficheros para la realizacion de la compilacion
SRC_BOT = src/$(GROUP)_bot.c
SRC_BOT += src/$(GROUP)_bot_connection.c
SRC_BOT += includes/$(GROUP)_bot_connection.h
SRC_BOT += src/$(GROUP)_bot_in_out.c
SRC_BOT += includes/$(GROUP)_bot_in_out.h
SRC_BOT += src/$(GROUP)_bot_handlers.c
SRC_BOT += includes/$(GROUP)_bot_handlers.h
SRC_BOT += srclib/$(GROUP)_tcp.c
SRC_BOT += includes/$(GROUP)_tcp.h

# Flags para el compilador
FFLAGS= -Wall -w -pedantic -pthread -rdynamic
REDESLIB= -lircredes -lirctad -lircinterface -lsoundredes
LIB = ./lib/
OBJ = ./obj/
SRCLIB = ./srclib/

all: clean bot_1 prelibreria libreria siri

bot_1:
	@mkdir -p "bot_1"
	@echo "\033[31mCompilando bot SIRI...\033[00m"
	@gcc -o ./bot_1/bot_1 $(SRC_BOT) $(FFLAGS) $(REDESLIB)
	@echo "\033[32m--> Bot compilado: ./bot_1/bot_1\033[00m"

doxy:
	@echo "\033[31mGenerando documentación...\033[00m"
	@doxygen Doxyfile  > /dev/null 2>&1
	@echo "\033[32m--> Documentación generada en: ./doc/html/index.html\033[00m"

prelibreria:
	@echo "\033[31mGenerando librería...\033[00m"
	@mkdir -p "obj"
	@mkdir -p "lib"

libreria: $(LIB)$(GROUP)_tcp.a

siri:
	./bot_1/bot_1 -a metis.ii.uam.es:6667 -c "#redes2" -n SiriBot

$(LIB)$(GROUP)_tcp.a: $(OBJ)$(GROUP)_tcp.o
	@ar -rv $@ $^ -lssl > /dev/null 2>&1
	@echo "\033[32m--> Libreria generada en: ./lib/$(GROUP)_tcp.a\033[00m"

$(OBJ)$(GROUP)_tcp.o: $(SRCLIB)$(GROUP)_tcp.c
	@$(CC) $(CCFLAGS) -o $@ -c $^

entrega: clean tar

clean:
	@echo "\033[31mBorrando ficheros temporales...\033[00m"
	rm -rf obj/*.o $(GROUP)
	rm -rf objlib/*.o $(GROUP)
	rm -rf "bot_1"
	rm -rf lib
	rm -rf obj
tar:
	@echo "\033[31mComprimiendo...\033[00m"
	tar --exclude .git -czf $(GROUP).tar.gz ../$(GROUP) --exclude $(GROUP).tar.gz
	@echo "\033[32mFichero comprimido: $(GROUP).tar.gz\033[00m"
