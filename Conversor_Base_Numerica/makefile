
#Makefile básico

BIN = a.out

CFLAGS += -Wall -Wextra -Werror -Wfatal-errors  
CFLAGS += -pedantic -pedantic-errors -ansi -std=c99 
#Por a flag que habilita o padrão C de 2018

SRC = main.c

$(BIN) : $(SRC)
	gcc $(CFLAGS) $(SRC) -o $(BIN) -lm

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(BIN)

.PHONY: run clean 

