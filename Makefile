# Uso:  make        (compila)   |   make run   (compila e executa)   |   make clean
# Rode SEMPRE a partir da pasta raiz do projeto (assets/ e saves/ sao caminhos relativos).
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Iinclude -IC:/raylib/include
SRC = src/main.c src/jogador.c src/inimigo.c src/projeteis.c src/fases.c src/arquivos.c src/sprites.c
TARGET = homem_aranha

ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    LDLIBS += -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
else ifeq ($(shell uname -s),Darwin)
    LDLIBS += -lraylib -framework IOKit -framework Cocoa -framework OpenGL
else
    CFLAGS += $(shell pkg-config --cflags raylib 2>/dev/null)
    LDLIBS += $(shell pkg-config --libs raylib 2>/dev/null || echo -lraylib) -lm
endif

all: $(TARGET)

$(TARGET): $(SRC) $(wildcard include/*.h)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	cmd /C "if exist $(TARGET) del /Q $(TARGET)"

.PHONY: all run clean