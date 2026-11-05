#ifndef INIMIGOS_H
#define INIMIGOS_H

#include "raylib.h"

#include <stdbool.h>

#define LINHAS_INIMIGOS 4
#define COLUNAS_INIMIGOS 9

typedef struct{// Estrutura que representa um inimigo
    Rectangle corpo;

    int linha;

    int coluna;

    int vida;
    
    struct Inimigo *proximo;
} Inimigo;

typedef struct{// Estrutura que representa a lista de inimigos
    Inimigo *inicio;

    int formacao[LINHAS_INIMIGOS][COLUNAS_INIMIGOS];

    int direcao;

    float temporizadorDisparo;
} ListaInimigos;

struct ListaProjeteis;

void InimigosInicializar(ListaInimigos *lista);

void InimigosLimpar(ListaInimigos *lista); // Limpa a lista de inimigos, removendo todos os elementos e liberando a memória

void InimigosCriarOnda(ListaInimigos *lista); // Cria uma nova onda de inimigos, preenchendo a lista com inimigos em formação

void InimigosAtualizar(ListaInimigos *lista, struct ListaProjeteis *projeteis,
                       float dt, int fase, int onda); 
        // Atualiza a posição e o estado dos inimigos na lista, verificando colisões com projéteis e removendo inimigos mortos

void InimigosDesenhar(const ListaInimigos *lista); 
// Desenha os inimigos na tela com base na lista de inimigos, utilizando a função DrawRectangleRec para cada inimigo ativo

int InimigosContar(const ListaInimigos *lista); // Retorna o número de inimigos ativos na lista

bool InimigosAtingiramBase(const ListaInimigos *lista, float limiteY); // Verifica se algum inimigo atingiu a base

void InimigosRemover(ListaInimigos *lista, Inimigo **elo); // Remove um inimigo específico da lista, liberando a memória e ajustando os ponteiros da lista

#endif
