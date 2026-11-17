#ifndef MENU_H

#define MENU_H

#include <stdbool.h>

typedef enum{ // O que o menu pede para o jogo fazer depois de atualizar
    MENU_NENHUMA = 0,
    MENU_JOGAR = 1,
    MENU_SAIR = 2
} AcaoMenu;

typedef struct Menu{
    int selecionado; // Opcao destacada (0 = JOGAR, 1 = COMO JOGAR, 2 = SAIR)

    bool mostrandoControles; // true enquanto o painel "COMO JOGAR" esta aberto
} Menu;

void MenuInicializar(Menu *menu); // Deixa o menu na opcao JOGAR, com o painel de controles fechado

AcaoMenu MenuAtualizar(Menu *menu); // Le teclado e mouse. Retorna MENU_JOGAR ou MENU_SAIR quando o jogador escolhe

void MenuDesenhar(const Menu *menu, int recorde, float rolagem); // Desenha o menu. "rolagem" anima o fundo

#endif
