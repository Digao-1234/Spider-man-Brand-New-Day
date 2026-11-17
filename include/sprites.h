#ifndef SPRITES_H

#define SPRITES_H

#include "raylib.h"

#include <stdbool.h>

typedef enum{ // Identificador de cada sprite do jogo
    SPR_JOGADOR_PARADO = 0,
    SPR_JOGADOR_ESQ,
    SPR_JOGADOR_DIR,
    SPR_NINJA,            // Inimigo da onda da fase 3
    SPR_GUARDA,           // Inimigo das ondas da fase 1 (guarda da CTU)
    SPR_GUARDA_APONTANDO, // O mesmo guarda, na pose de atirar
    SPR_BOSS1,
    SPR_BOSS2,
    SPR_BOSS3,
    SPR_TEIA,
    SPR_TIRO_NINJA,
    SPR_TIRO_GUARDA,
    SPR_TIRO_BOSS1,
    SPR_TIRO_BOSS2,
    SPR_TIRO_BOSS3,
    SPR_FUNDO_FASE1,      // Cenarios: um por fase (SPR_FUNDO_FASE1 + fase - 1)
    SPR_FUNDO_FASE2,
    SPR_FUNDO_FASE3,
    SPR_TOTAL
} SpriteId;

void SpritesCarregar(void); // Carrega todas as texturas.

void SpritesDescarregar(void); // Libera as texturas. 

bool SpritesDesenhar(SpriteId id, float x, float y, float altura, float rotacao, Color tint);
// Desenha o sprite especificado na posição (x, y) com a altura e rotação especificadas, aplicando a cor de tintura.

#endif