#ifndef FASES_H
#define FASES_H

#include "raylib.h"

#include <stdbool.h>

struct ListaInimigos;

struct ListaProjeteis;

typedef enum{
    JOGO_EM_ANDAMENTO = 0,
    JOGO_VITORIA = 1,
    JOGO_DERROTA = 2
} EstadoJogo;

typedef struct{ // Estrutura que representa o chefe da fase
    Rectangle corpo;
    
    int vida;

    int vidaMaxima;

    float velocidade;

    float intervaloDisparo;

    int tipo; // 1 = chefe da fase 1; 2 = miniboss; 3 = chefe final 
    
    int direcao;
    
    bool ativo;
} Chefe;

typedef struct{ // Estrutura que representa as fases do jogo
    int faseAtual;

    int ondaFaseUm;// Número de ondas na fase 1

    Chefe chefe;
} Fases;

void FasesInicializar(Fases *fases, struct ListaInimigos *inimigos, struct ListaProjeteis *projeteis); 
                                    // Inicializa a estrutura de fases, definindo a fase atual, o chefe e os inimigos.

void FasesAtualizar(Fases *fases, struct ListaInimigos *inimigos, struct ListaProjeteis *projeteis, float dt,
                    EstadoJogo *estado); // Atualiza o estado do jogo com base na fase atual, inimigos e projéteis.

void FasesDesenhar(const Fases *fases); // Desenha os elementos visuais relacionados à fase atual

void FasesReiniciar(Fases *fases, struct ListaInimigos *inimigos, struct ListaProjeteis *projeteis); // Reinicia a fase atual

#endif