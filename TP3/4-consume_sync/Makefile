
# ----------------------------------------------------------------------------
# Déclaration de variables
# ----------------------------------------------------------------------------

# la commande $(wildcard src/*.c) nous renvoie la liste des fichiers '*.c' dans le dossier source
SRC := $(wildcard *.c)
# On prend tous les fichiers src/*.c dans la variable SRC, on remplace par build/*.o
OBJ := $(patsubst %.c,%.o,$(SRC))
# Le nom du programme final
BIN := consume_sync
LOG := $(BIN).log
PGM := *.pgm
TXT := *.txt

# Nombre de threads à utiliser 
THREAD_NUM = 1 2 3 4 5 6 7 8 9 10

# Notez ici les paramètres de compilation
CFLAGS := -O0 -g 	# pas d'optimisation, ajouter les symboles de debug
CFLAGS += -Iinc/ 	# headers .h dans inc/
CFLAGS += -Wall		# afficher tous les warnings

# Notez ici les paramètres de liaison (librairies nécessaires)
LDFLAGS := -lm -lpthread -fopenmp

# Commandes utilisées pour compiler & linker
CC := gcc
LD := gcc

# ----------------------------------------------------------------------------
# Règles de compilation
# ----------------------------------------------------------------------------

# Structure générale d'une règle: pour construire 'cible', il faut d'abord
# construire ou récupérer plusieurs prérequis, puis appliquer une ou plusieurs 
# commandes.
#
# cible: dépendance1 dépendance2 ...
# 	commande1 (ligne commence par TAB ; attention pas d'espace!)
#	commande2
# 

# Quelques variables automatiques dans make:
# % : pattern matching
# $@ : la cible de la règle en cours
# $^ : toutes les dépendances listées
# $< : la 1° dépendance listée


# Règle de compilation pour le programme principal:
$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(LDFLAGS)

# pour nettoyer: supprimer les fichiers générés
clean:
	rm -f $(OBJ) $(BIN) $(LOG) $(PGM) $(TXT)
  
# pour mesurer le temps en variant le nombre de threads num de 1 à 8
# for num in [1 2 3 4 5 6 7 8]:
#	time -a -o ./tutorial.log ./tutorial num
time: $(BIN)
	rm -f $(LOG)
	$(foreach num, $(THREAD_NUM), echo "Avec $(num) threads:" >> $(LOG) && time -p -a -o $(LOG) ./$(BIN) $(num);)

# on indique que les cibles des règles 'clean' et 'time' ne sont pas des fichiers
.PHONY: clean time
