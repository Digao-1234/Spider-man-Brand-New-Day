#ifndef SPRITES_H

#define SPRITES_H

#include "raylib.h"

#include <stdbool.h>

typedef enum{ // Identificador de cada sprite do jogo
    SPR_JOGADOR_PARADO = 0,
    SPR_JOGADOR_ESQ,
    SPR_JOGADOR_DIR,
    SPR_INIMIGO,
    SPR_BOSS1,
    SPR_BOSS2,
    SPR_BOSS3,
    SPR_TEIA,
    SPR_TIRO_INIMIGO,
    SPR_TIRO_BOSS1,
    SPR_TIRO_BOSS2,
    SPR_TIRO_BOSS3,
    SPR_TOTAL
} SpriteId;

void SpritesCarregar(void); // Carrega todas as texturas.

void SpritesDescarregar(void); // Libera as texturas. 

bool SpritesDesenhar(SpriteId id, float x, float y, float altura, float rotacao, Color tint);
// Desenha o sprite especificado na posição (x, y) com a altura e rotação especificadas, aplicando a cor de tintura.
#endif