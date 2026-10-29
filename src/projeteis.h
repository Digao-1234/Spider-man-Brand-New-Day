#ifndef PROJETEIS_H
#define PROJETEIS_H

#include "raylib.h"
#include <stdbool.h>

typedef enum{
    PROJETIL_TEIA = 0,
    PROJETIL_INIMIGO = 1
} TipoProjetil;

typedef struct{
    Rectangle corpo;

    float velocidadeY;

    TipoProjetil tipo;

    struct Projetil *proximo;

} Projetil;

typedef struct{
    Projetil *inicio;

} ListaProjeteis;

struct ListaInimigos;

struct Fases;

struct Jogador;

void ProjeteisInicializar(ListaProjeteis *lista);// Inicializa a lista de projéteis

void ProjeteisAdicionar(ListaProjeteis *lista, Rectangle corpo, float velocidadeY, TipoProjetil tipo);// Adiciona um projétil à lista

void ProjeteisAtualizar(ListaProjeteis *lista, float dt);// Atualiza a posição dos projéteis na lista

void ProjeteisDesenhar(const ListaProjeteis *lista);// Desenha os projéteis na tela

void ProjeteisLimpar(ListaProjeteis *lista);// Limpa a lista de projéteis, liberando a memória alocada

void ProjeteisResolverColisoes(ListaProjeteis *projeteis, struct ListaInimigos *inimigos, struct Fases *fases,
                               struct Jogador *jogador, int *pontuacao);// Verifica e resolve as colisões entre projéteis, 
                                                                        //inimigos, fases e jogador

#endif