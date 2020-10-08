

BIN := main
SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))

CFLAGS := -O0 -g 	# pas d'optimisation, ajouter les symboles de debug
CFLAGS += -Wall		# afficher tous les warnings
CFLAGS += -Iinc/ 	# headers .h dans inc/

LDFLAGS := -lm

CC := gcc
LD := gcc

$(BIN): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJ) $(BIN) $(LOG)
	rm -f ./*.pgm
	rm -f out/*

test: $(BIN)
	time -p ./$(BIN)

.PHONY: clean test
