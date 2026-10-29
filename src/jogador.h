#ifndef JOGADOR_H
#define JOGADOR_H

#include "raylib.h"

#include <stdbool.h>

struct ListaProjeteis;

typedef struct{
    Rectangle corpo;

    float velocidade;

    float intervaloDisparo;

    float invulnerabilidade;

    int vidas;
} Jogador;

void JogadorInicializar(Jogador *jogador);

void JogadorAtualizar(Jogador *jogador, struct ListaProjeteis *projeteis, float dt, bool podeJogar);
                                            // Atualiza o estado do jogador, incluindo movimento, disparo e invulnerabilidade

void JogadorDesenhar(const Jogador *jogador);// Desenha o jogador na tela, incluindo efeitos de invulnerabilidade

#endif