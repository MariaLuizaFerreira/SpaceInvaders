.PHONY: all clean

PASTINHA_CONJUNTA = ../
CC = $(PASTINHA_CONJUNTA)/MINGW/bin/gcc
PATH_ALLEGRO = $(PASTINHA_CONJUNTA)/allegro-5.0.10-mingw-4.7.0
LIBS = -lallegro-5.0.10-monolith-mt

# Adicione todos os seus arquivos .o aqui
OBJECTS = tp.o game_objects.o 

# Adicione todos os diretórios de include para seus cabeçalhos
INCLUDE_DIRS = -I. -I$(PATH_ALLEGRO)/include

# Define flags de compilação e linkagem
CFLAGS = -O2 $(INCLUDE_DIRS)
LDFLAGS = -L $(PATH_ALLEGRO)/lib $(LIBS)

all: tp.exe
    
tp.exe: $(OBJECTS)
	$(CC) $(CFLAGS) -o tp.exe $(OBJECTS) $(LDFLAGS)

# Regra genérica para compilar arquivos .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) tp.exe