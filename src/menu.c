#include "menu.h"

#include "raylib.h"

#include "config.h"

#include "sprites.h"

#include <math.h>

#define MENU_NUM_OPCOES 3
#define MENU_BOTAO_LARGURA 320
#define MENU_BOTAO_ALTURA 40
#define MENU_BOTAO_Y0 436   // Botoes mais abaixo para nao cobrir o Homem-Aranha do fundo. Para subir/descer todos, mude so este valor
#define MENU_BOTAO_PASSO 50

static const char *textosOpcoes[MENU_NUM_OPCOES] = { "JOGAR", "COMO JOGAR", "SAIR" };

static Rectangle MenuBotao(int i){
    // Retangulo do botao i. Usado tanto para desenhar quanto para detectar o mouse, assim os dois nunca ficam desalinhados

    return (Rectangle){ (LARGURA_TELA - MENU_BOTAO_LARGURA) / 2.0f,
                        (float)(MENU_BOTAO_Y0 + i * MENU_BOTAO_PASSO),
                        (float)MENU_BOTAO_LARGURA,
                        (float)MENU_BOTAO_ALTURA };
}

static void DesenharTextoCentralizado(const char *texto, int y, int tamanho, Color cor){
    DrawText(texto, (LARGURA_TELA - MeasureText(texto, tamanho)) / 2, y, tamanho, cor);
}

static MenuAcao MenuAtivarOpcao(Menu *menu){
    // Executa o botao selecionado

    switch (menu->opcao){
        case 0:
            return MENU_ACAO_JOGAR;

        case 1:
            menu->mostrandoControles = true;
            return MENU_ACAO_NENHUMA;

        default:
            return MENU_ACAO_SAIR;
    }
}

void MenuInicializar(Menu *menu){
    menu->opcao = 0;

    menu->mostrandoControles = false;

    menu->tempo = 0.0f;
}

MenuAcao MenuAtualizar(Menu *menu, float dt){
    menu->tempo += dt;

    if (menu->mostrandoControles){
        // Na tela de controles qualquer confirmacao volta ao menu

        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            menu->mostrandoControles = false;
        }
        return MENU_ACAO_NENHUMA;
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){
        menu->opcao = (menu->opcao + 1) % MENU_NUM_OPCOES;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
        menu->opcao = (menu->opcao + MENU_NUM_OPCOES - 1) % MENU_NUM_OPCOES;
    }

    bool clicou = false;

    Vector2 mouse = GetMousePosition();

    Vector2 movimento = GetMouseDelta();

    for (int i = 0; i < MENU_NUM_OPCOES; i++){
        if (!CheckCollisionPointRec(mouse, MenuBotao(i))){
            continue;
        }

        if (movimento.x != 0.0f || movimento.y != 0.0f){
            menu->opcao = i;
            // So troca a selecao quando o mouse se mexe, senao um mouse parado em cima de um botao "brigaria" com o teclado
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            menu->opcao = i;

            clicou = true;
        }
    }

    if (IsKeyPressed(KEY_ENTER) || clicou){
        // So o ENTER confirma (nao o ESPACO): o ESPACO e a tecla de atirar e o tiro sairia assim que a partida comecasse

        return MenuAtivarOpcao(menu);
    }

    if (IsKeyPressed(KEY_ESCAPE)){
        return MENU_ACAO_SAIR;
    }

    return MENU_ACAO_NENHUMA;
}

static void MenuDesenharControles(void){
    DesenharTextoCentralizado("COMO JOGAR", 80, 44, RAYWHITE);

    DrawRectangle(180, 160, LARGURA_TELA - 360, 300, (Color){ 8, 12, 29, 210 });

    DrawRectangleLines(180, 160, LARGURA_TELA - 360, 300, (Color){ 70, 90, 130, 255 });

    const char *linhas[] = {
        "A / D ou setas    mover",
        "ESPACO            atirar teia (segure para continuar atirando)",
        "P                 pausar / continuar",
        "ENTER             jogar de novo (na tela de vitoria/derrota)",
        "ESC               voltar ao menu"
    };

    for (int i = 0; i < 5; i++){
        DrawText(linhas[i], 210, 190 + i * 48, 18, RAYWHITE);
    }

    DesenharTextoCentralizado("ENTER ou ESC para voltar", ALTURA_TELA - 50, 18, LIGHTGRAY);
}

void MenuDesenhar(const Menu *menu, int recorde){
    ClearBackground((Color){ 8, 12, 29, 255 });

    // Reaproveita o cenario da fase 1. Se o PNG nao carregou, cai no fundo de estrelas (igual ao da partida)
    // Fundo proprio do menu (assets/sprites/fundo_menu.png). Se nao existir, usa o cenario da fase 1
    bool temFundo = SpritesDesenharFundo(SPR_FUNDO_MENU, (float)LARGURA_TELA, (float)ALTURA_TELA, WHITE);

    if (!temFundo){
        temFundo = SpritesDesenharFundo(SPR_FUNDO_FASE1, (float)LARGURA_TELA, (float)ALTURA_TELA, WHITE);
    }

    if (temFundo){
        DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){ 0, 0, 0, 140 });
        // Escurece o cenario para o titulo e os botoes ficarem legiveis
    }
    else {
        for (int i = 0; i < 70; i++){
            DrawPixel((i * 137 + 29) % LARGURA_TELA, (i * 83 + 17) % ALTURA_TELA, (Color){ 72, 85, 125, 255 });
        }
    }

    if (menu->mostrandoControles){
        MenuDesenharControles();

        return;
    }

    SpritesDesenhar(SPR_JOGADOR_PARADO, 830.0f, (float)ALTURA_TELA - 30.0f, 220.0f, 0.0f, WHITE);
    // Se o sprite nao carregou, simplesmente nao aparece: o menu continua funcionando

    DesenharTextoCentralizado("HOMEM-ARANHA", 112, 58, (Color){ 0, 0, 0, 200 }); // sombra
    DesenharTextoCentralizado("HOMEM-ARANHA", 108, 58, (Color){ 220, 30, 45, 255 });
    DesenharTextoCentralizado("UM NOVO DIA", 178, 28, RAYWHITE);

    for (int i = 0; i < MENU_NUM_OPCOES; i++){
        Rectangle botao = MenuBotao(i);

        bool selecionado = (i == menu->opcao);

        Color fundo = selecionado ? (Color){ 200, 30, 40, (unsigned char)(215.0f + 35.0f * sinf(menu->tempo * 5.0f)) }
                                  : (Color){ 20, 30, 60, 190 };

        DrawRectangleRec(botao, fundo);

        DrawRectangleLinesEx(botao, 2.0f, selecionado ? RAYWHITE : (Color){ 70, 90, 130, 255 });

        int tamanho = 24;

        int larguraTexto = MeasureText(textosOpcoes[i], tamanho);

        DrawText(textosOpcoes[i],
                 (int)(botao.x + (botao.width - larguraTexto) / 2.0f),
                 (int)(botao.y + (botao.height - tamanho) / 2.0f),
                 tamanho,
                 selecionado ? RAYWHITE : LIGHTGRAY);
    }

    DesenharTextoCentralizado(TextFormat("RECORDE: %d", recorde), 232, 20, GOLD);// Logo abaixo do titulo (os botoes ficam la embaixo)

    DesenharTextoCentralizado("W/S ou setas: escolher   |   ENTER ou clique: confirmar   |   ESC: sair", ALTURA_TELA - 28, 15, LIGHTGRAY);
}
