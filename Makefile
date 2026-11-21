# Dado Burger (raylib)
#   make        compila
#   make run    compila e roda com pedidos.txt e historico.txt
#   make clean  apaga o executavel

CC     = gcc
CFLAGS = -Wall -Wextra -O2
SRC    = $(wildcard src/*.c)
HDR    = $(wildcard include/*.h)
ALVO   = dado_burger

ifeq ($(OS),Windows_NT)
    ALVO = dado_burger.exe
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
else ifeq ($(shell uname -s),Darwin)
    LIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

$(ALVO): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LIBS)

run: $(ALVO)
	./$(ALVO) pedidos.txt historico.txt

clean:
	rm -f $(ALVO)

.PHONY: run clean
