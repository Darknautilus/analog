MODE=DEBUG
CC=g++
CFLAGS=-std=c++11
LDFLAGS=
EXEC=AnaLog
SRC=AnaLog.cpp LigneLog.cpp Rapport.cpp 
OBJ=$(SRC:.cpp=.o)

ifeq ($(MODE),DEBUG)
	CFLAGS:=$(CFLAGS) -g
endif

all: $(EXEC)

AnaLog: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

AnaLog.o: Config.h LecteurLigneFichier.h Rapport.h LigneLog.h

Rapport.o: Config.h LecteurLigneFichier.h LigneLog.h

LigneLog.o: Config.h LecteurLigneFichier.h Rapport.h

clean:
	@rm -rf *.o

mrproper: clean
	@rm -rf $(EXEC)
