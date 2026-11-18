#ifndef MENU_H

#define MENU_H

#include <stdbool.h>

typedef enum{ // O que o menu pede para o main fazer neste quadro
    MENU_ACAO_NENHUMA = 0,
    MENU_ACAO_JOGAR,
    MENU_ACAO_SAIR
} MenuAcao;

typedef struct Menu{
    int opcao; // Botao selecionado (0 = JOGAR, 1 = COMO JOGAR, 2 = SAIR)

    bool mostrandoControles; // true = esta na tela "COMO JOGAR"

    float tempo; // Segundos desde que o menu abriu (so serve para a animacao do botao)
} Menu;

void MenuInicializar(Menu *menu); // Volta o menu ao estado inicial (botao JOGAR selecionado)

MenuAcao MenuAtualizar(Menu *menu, float dt); // Le teclado e mouse. Retorna a acao que o main deve executar

void MenuDesenhar(const Menu *menu, int recorde); // Desenha o menu (ou a tela de controles) e o recorde salvo

#endif
