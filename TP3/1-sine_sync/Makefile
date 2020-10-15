
# ----------------------------------------------------------------------------
# D�claration de variables
# ----------------------------------------------------------------------------

# la commande $(wildcard src/*.c) nous renvoie la liste des fichiers '*.c' dans le dossier source
SRC := $(wildcard *.c)
# On prend tous les fichiers src/*.c dans la variable SRC, on remplace par build/*.o
OBJ := $(patsubst %.c,%.o,$(SRC))
# Le nom du programme final
BIN := sine_sync
LOG := $(BIN).log
PGM := *.pgm
TXT := *.txt
OUT := *.out

# Nombre de threads � utiliser 
THREAD_NUM = 1 2 3 4 5 6 7 8 9 10

# Notez ici les param�tres de compilation
CFLAGS := -O0 -g 	# pas d'optimisation, ajouter les symboles de debug
CFLAGS += -Iinc/ 	# headers .h dans inc/
CFLAGS += -Wall		# afficher tous les warnings

# Notez ici les param�tres de liaison (librairies n�cessaires)
LDFLAGS := -lm -lpthread	

# Commandes utilis�es pour compiler & linker
CC := gcc
LD := gcc

# ----------------------------------------------------------------------------
# R�gles de compilation
# ----------------------------------------------------------------------------

# Structure g�n�rale d'une r�gle: pour construire 'cible', il faut d'abord
# construire ou r�cup�rer plusieurs pr�requis, puis appliquer une ou plusieurs 
# commandes.
#
# cible: d�pendance1 d�pendance2 ...
# 	commande1 (ligne commence par TAB ; attention pas d'espace!)
#	commande2
# 

# Quelques variables automatiques dans make:
# % : pattern matching
# $@ : la cible de la r�gle en cours
# $^ : toutes les d�pendances list�es
# $< : la 1� d�pendance list�e

# R�gle de compilation pour le programme principal:
$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(LDFLAGS)

# pour nettoyer: supprimer les fichiers g�n�r�s
clean:
	rm -f $(OBJ) $(BIN) $(LOG) $(PGM) $(TXT) $(OUT)
  
# pour mesurer le temps en variant le nombre de threads num de 1 � 8
# for num in [1 2 3 4 5 6 7 8]:
#	time -a -o ./tutorial.log ./tutorial num
time: $(BIN)
	rm -f $(LOG)
	$(foreach num, $(THREAD_NUM), echo "Avec $(num) threads:" >> $(LOG) && time -p -a -o $(LOG) ./$(BIN) $(num);)

# on indique que les cibles des r�gles 'clean' et 'time' ne sont pas des fichiers
.PHONY: clean time
