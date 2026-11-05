#include "projeteis.h"

#include "inimigos.h"

#include "fases.h"

#include "jogador.h"

#include "config.h"

#include <stdlib.h>


void ProjeteisInicializar(ListaProjeteis *lista){
    if (lista != NULL){
         lista->inicio = NULL;// Inicializa a lista de projéteis, definindo o ponteiro de início como NULL
    }
}

void ProjeteisAdicionar(ListaProjeteis *lista, Rectangle corpo, float velocidadeY, TipoProjetil tipo){

    if (lista == NULL){// Verifica se a lista é nula antes de adicionar um projétil
         return;
    }

    Projetil *novo = (Projetil *)malloc(sizeof(Projetil));// Aloca memória para um novo projétil

    if (novo == NULL){// Verifica se a alocação de memória foi bem-sucedida
         return;
    }

    novo->corpo = corpo;// Define o corpo do projétil 

    novo->velocidadeY = velocidadeY;// Define a velocidade vertical do projétil

    novo->tipo = tipo;// Define o tipo do projétil (teia ou inimigo)

    novo->proximo = lista->inicio;// Define o próximo projétil como o início da lista atual

    lista->inicio = novo;// Atualiza o início da lista para apontar para o novo projétil
}

void ProjeteisLimpar(ListaProjeteis *lista){

    if (lista == NULL){
         return;// Verifica se a lista é nula antes de limpar os projéteis
    }

    Projetil *atual = lista->inicio;// Define um ponteiro para percorrer a lista de projéteis

    while (atual != NULL){
        Projetil *proximo = atual->proximo;// Armazena o próximo projétil antes de liberar a memória do atual

        free(atual);

        atual = proximo;// Avança para o próximo projétil na lista
    }

    lista->inicio = NULL;// Define o início da lista como NULL após liberar todos os projéteis
}

void ProjeteisAtualizar(ListaProjeteis *lista, float dt){

    if (lista == NULL){
         return;// Verifica se a lista é nula antes de atualizar os projéteis
    }

    Projetil **elo = &lista->inicio;// Define um ponteiro para ponteiro para percorrer a lista de projéteis

    while (*elo != NULL){
        Projetil *p = *elo;// Define um ponteiro para o projétil atual

        p->corpo.y += p->velocidadeY * dt;// Atualiza a posição vertical do projétil

        bool foraDaTela = (p->corpo.y + p->corpo.height < 0.0f) || (p->corpo.y > ALTURA_TELA);// Verifica se o projétil saiu da tela

        if (foraDaTela){
            // Se o projétil estiver fora da tela, remove-o da lista e libera a memória
            *elo = p->proximo;

            free(p);
        } 

        else{
            elo = &p->proximo;// Avança para o próximo projétil na lista
        }
    }
}

void ProjeteisDesenhar(const ListaProjeteis *lista){
    if (lista == NULL){
         return;// Verifica se a lista é nula antes de desenhar os projéteis
    }

    for (const Projetil *p = lista->inicio; p != NULL; p = p->proximo){
        if (p->tipo == PROJETIL_TEIA){
            DrawRectangleRounded(p->corpo, 0.45f, 4, SKYBLUE);// Desenha o projétil de teia com arredondamento e cor azul

            DrawRectangle((int)p->corpo.x + 2, (int)p->corpo.y, 2, (int)p->corpo.height, WHITE);
            // Desenha uma linha branca no centro do projétil de teia para dar efeito visual
        } 

        else{
            DrawRectangleRounded(p->corpo, 0.35f, 4, ORANGE);// Desenha o projétil inimigo com arredondamento e cor laranja
        }
    }
}

void ProjeteisResolverColisoes(ListaProjeteis *projeteis, ListaInimigos *inimigos, Fases *fases, Jogador *jogador, int *pontuacao){

    if (projeteis == NULL || inimigos == NULL || fases == NULL || jogador == NULL || pontuacao == NULL){
         return;// Verifica se algum dos ponteiros é nulo antes de resolver as colisões
        }

    Projetil **eloProjetil = &projeteis->inicio;// Define um ponteiro para ponteiro para percorrer a lista de projéteis

    while (*eloProjetil != NULL){// Enquanto houver projéteis na lista, verifica colisões com inimigos, chefe e jogador
        
        Projetil *p = *eloProjetil;// Define um ponteiro para o projétil atual

        bool removerProjetil = false;// Define uma variável para indicar se o projétil deve ser removido após a colisão

        if (p->tipo == PROJETIL_TEIA){// Se o projétil for do tipo teia, verifica colisões com inimigos e chefe
            
            Inimigo **eloInimigo = &inimigos->inicio;// Define um ponteiro para ponteiro para percorrer a lista de inimigos

            while (*eloInimigo != NULL){// Enquanto houver inimigos na lista, verifica colisões com o projétil de teia
                
                Inimigo *inimigo = *eloInimigo;// Define um ponteiro para o inimigo atual

                if (CheckCollisionRecs(p->corpo, inimigo->corpo)){// Verifica se houve colisão entre o projétil de teia e o inimigo
                    inimigo->vida--;// Reduz a vida do inimigo em 1

                    removerProjetil = true;// Define que o projétil deve ser removido após a colisão

                    if (inimigo->vida <= 0){// Se a vida do inimigo chegar a 0 ou menos, remove o inimigo da lista
                        InimigosRemover(inimigos, eloInimigo);

                        *pontuacao += 10;// Adiciona 10 pontos à pontuação do jogador por eliminar o inimigo
                    }
                    break;
                }
                eloInimigo = &inimigo->proximo;// Avança para o próximo inimigo na lista
            }

            if (!removerProjetil && fases->chefe.ativo && CheckCollisionRecs(p->corpo, fases->chefe.corpo)){// Verifica colisão com o chefe da fase se o projétil ainda não foi removido e o chefe estiver ativo
                fases->chefe.vida--;// Reduz a vida do chefe em 1

                removerProjetil = true;// Define que o projétil deve ser removido após a colisão com o chefe

                if (fases->chefe.vida <= 0){// Se a vida do chefe chegar a 0 ou menos, marca o chefe como inativo e adiciona pontos à pontuação do jogador
                    
                    fases->chefe.vida = 0;// Garante que a vida do chefe não seja negativa

                    fases->chefe.ativo = false;// Marca o chefe como inativo, indicando que ele foi derrotado

                    if (fases->chefe.tipo == 1){
                         *pontuacao += 250;// Adiciona 250 pontos à pontuação do jogador por derrotar o chefe do tipo 1
                    }
                    else if (fases->chefe.tipo == 2){
                         *pontuacao += 400;// Adiciona 400 pontos à pontuação do jogador por derrotar o chefe do tipo 2
                    }
                    else {
                         *pontuacao += 750;// Adiciona 750 pontos à pontuação do jogador por derrotar o chefe do tipo 3
                    }
                }
            }
        } 

        else if (CheckCollisionRecs(p->corpo, jogador->corpo)){// Se o projétil for do tipo inimigo, verifica colisão com o jogador

            if (jogador->invulnerabilidade <= 0.0f){// Se o jogador não estiver invulnerável, reduz suas vidas e ativa a invulnerabilidade temporária
                
                jogador->vidas--;// Reduz o número de vidas do jogador em 1

                jogador->invulnerabilidade = 1.1f;// Ativa a invulnerabilidade do jogador por 1.1 segundos após ser atingido
            }

            removerProjetil = true;// Define que o projétil deve ser removido após a colisão com o jogador
        }

        if (removerProjetil){// Se o projétil deve ser removido, ajusta os ponteiros da lista e libera a memória do projétil
            *eloProjetil = p->proximo;
            free(p);
        }

        else{// Se o projétil não deve ser removido, avança para o próximo projétil na lista
            eloProjetil = &p->proximo;
        }
    }
}