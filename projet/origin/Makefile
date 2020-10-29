
BIN := main
SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))

CFLAGS := -O0 -g 	# pas d'optimisation, ajouter les symboles de debug
CFLAGS += -Wall		# afficher tous les warnings
CFLAGS += -Iinc/ 	# headers .h dans inc/
CFLAGS += -fopenmp
# CFLAGS += -DNDEBUG # no debug

LDFLAGS := -lm -pthread -fopenmp 

CC := gcc
LD := gcc

$(BIN): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	# clean compilation outputs
	rm -f $(OBJ) $(BIN) $(LOG)
	# clean the output of previous executions
	rm -f ./*.pgm 
	rm -f ./*.ppm

.PHONY: clean test
