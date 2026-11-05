#include "jogador.h"

#include "projeteis.h"

#include "config.h"

#include <stddef.h>


void JogadorInicializar(Jogador *jogador){
    if (jogador == NULL){
         return;
    }
    jogador->corpo = (Rectangle){// Posição inicial do jogador
        LARGURA_TELA / 2.0f - 24.0f, ALTURA_TELA - 83.0f, 48.0f, 27.0f
    };
    jogador->velocidade = 390.0f;// definição da velocidade do jogador
    jogador->intervaloDisparo = 0.0f;// definição do intervalo de disparo do jogador
    jogador->invulnerabilidade = 0.0f;// definição do tempo de invulnerabilidade do jogador
    jogador->vidas = 3;// número de vidas do jogador
}

void JogadorAtualizar(Jogador *jogador, ListaProjeteis *projeteis, float dt, bool podeJogar){
    if (jogador == NULL || projeteis == NULL || !podeJogar){
         return;
    }

    if (jogador->intervaloDisparo > 0.0f){// Intervalo de disparo do jogador
         jogador->intervaloDisparo -= dt;
    }
    if (jogador->invulnerabilidade > 0.0f){// Invulnerabilidade do jogador
         jogador->invulnerabilidade -= dt;
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)){// Movimento do jogador esquerda
        jogador->corpo.x -= jogador->velocidade * dt;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)){// Movimento do jogador direita
        jogador->corpo.x += jogador->velocidade * dt;
    }

    if (jogador->corpo.x < 12.0f){
         jogador->corpo.x = 12.0f;// Limite da tela para o jogador
    }
    if (jogador->corpo.x + jogador->corpo.width > LARGURA_TELA - 12.0f){
         jogador->corpo.x = LARGURA_TELA - 12.0f - jogador->corpo.width;// Limite da tela para o jogador
    }

    if (IsKeyDown(KEY_SPACE) && jogador->intervaloDisparo <= 0.0f) {// Disparo de teia
        Rectangle teia = {
            jogador->corpo.x + jogador->corpo.width / 2.0f - 3.0f,// Posição do projétil (teia) em relação ao jogador
            jogador->corpo.y - 15.0f, 6.0f, 17.0f// Tamanho do projétil (teia)
        };
        ProjeteisAdicionar(projeteis, teia, -540.0f, PROJETIL_TEIA);// Adiciona o projétil (teia) à lista de projéteis
        jogador->intervaloDisparo = 0.23f;// Intervalo de disparo do jogador
    }
}

void JogadorDesenhar(const Jogador *jogador){
    if (jogador == NULL){
         return;
    }
    Color cor = RED;// Cor padrão do jogador
    if (jogador->invulnerabilidade > 0.0f && ((int)(GetTime() * 14.0) % 2 == 0)){
        cor = RAYWHITE;// Cor do jogador quando invulnerável (pisca)
    }

    Rectangle p = jogador->corpo; //desenho do jogador
    
    DrawRectangleRounded(p, 0.22f, 5, cor);// Corpo do jogador

    DrawRectangle((int)p.x + 5, (int)p.y + 5, (int)p.width - 10, 5, MAROON);

    DrawRectangle((int)p.x + 9, (int)p.y + 8, 10, 5, SKYBLUE);

    DrawRectangle((int)p.x + (int)p.width - 19, (int)p.y + 8, 10, 5, SKYBLUE);

    DrawTriangle((Vector2){p.x + 7, p.y + 4},
                 (Vector2){p.x - 1, p.y + 13},
                 (Vector2){p.x + 8, p.y + 19}, cor);// Braço esquerdo do jogador

    DrawTriangle((Vector2){p.x + p.width - 7, p.y + 4},
                 (Vector2){p.x + p.width - 8, p.y + 19},
                 (Vector2){p.x + p.width + 1, p.y + 13}, cor);// Braço direito do jogador
}
