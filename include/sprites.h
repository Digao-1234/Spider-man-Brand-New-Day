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
    SPR_GUARDA, // Guarda da CTU parado
    SPR_GUARDA_MIRANDO, // Guarda levantando a arma
    SPR_GUARDA_FRENTE, // Guarda apontando a arma para o jogador (logo depois de atirar)
    SPR_TIRO_GUARDA, // Tiro de energia do guarda
    SPR_TOTAL
} SpriteId;

void SpritesCarregar(void); // Carrega todas as texturas.

void SpritesDescarregar(void); // Libera as texturas. 

bool SpritesDesenhar(SpriteId id, float x, float y, float altura, float rotacao, Color tint);
// Desenha o sprite especificado na posição (x, y) com a altura e rotação especificadas, aplicando a cor de tintura.
bool SpritesDesenharFundo(int fase, float rolagem);
// Desenha o fundo da fase (1 a 3) rolando na vertical. Retorna false se o PNG nao carregou (quem chamou desenha outro fundo).

#endif