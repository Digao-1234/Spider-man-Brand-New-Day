#ifndef PROJETEIS_H

#define PROJETEIS_H

#include "raylib.h"

#include <stdbool.h>

typedef enum TipoProjetil{
    PROJETIL_TEIA = 0,
    PROJETIL_INIMIGO = 1,
    PROJETIL_BOSS1 = 2, // Tiros dos chefes: machucam o jogador, so o desenho muda
    PROJETIL_BOSS2 = 3,
    PROJETIL_BOSS3 = 4,
    PROJETIL_GUARDA = 5 // Tiro dos guardas (fase 1): machuca o jogador como os outros, so o desenho muda
} TipoProjetil;

// Tiro do Boss 2 (Hulk): um unico disparo grande. O DESENHO e a COLISAO usam este mesmo tamanho.
// Para aumentar/diminuir o tiro, mude so TIRO_BOSS2_ALTURA: a largura acompanha a proporcao do sprite
// (recorte de projetil_Boss2.png em sprites.c: largura 455 x altura 777), e o hitbox e o desenho mudam juntos.
#define TIRO_BOSS2_ALTURA 110.0f
#define TIRO_BOSS2_LARGURA (TIRO_BOSS2_ALTURA * 455.0f / 777.0f)

typedef struct Projetil{
    Rectangle corpo;

    float velocidadeY;

    TipoProjetil tipo;

    struct Projetil *proximo;

} Projetil;

typedef struct ListaProjeteis{
    Projetil *inicio;

} ListaProjeteis;

struct ListaInimigos;

struct Fases;

struct Jogador;

void ProjeteisInicializar(ListaProjeteis *lista);// Inicializa a lista de projéteis

void ProjeteisAdicionar(ListaProjeteis *lista, Rectangle corpo, float velocidadeY, TipoProjetil tipo);// Adiciona um projétil à lista

void ProjeteisAtualizar(ListaProjeteis *lista, float dt);// Atualiza a posição dos projéteis na lista

void ProjeteisDesenhar(const ListaProjeteis *lista);// Desenha os projéteis na tela

void ProjeteisLimpar(ListaProjeteis *lista);// Limpa a lista de projéteis, liberando a memória

void ProjeteisResolverColisoes(ListaProjeteis *projeteis, struct ListaInimigos *inimigos, struct Fases *fases,
                               struct Jogador *jogador, int *pontuacao);// Verifica e resolve as colisões entre projéteis, 
                                                                        //inimigos, fases e jogador

#endif