#include "menu.h"

#include "raylib.h"

#include "config.h"

#include "sprites.h"

#include <math.h>

#include <stddef.h>

#define TOTAL_OPCOES 3

#define OPCAO_LARGURA 320.0f

#define OPCAO_ALTURA 46.0f

#define OPCAO_ESPACO 14.0f

#define OPCAO_TOPO 300.0f

static const char *textosOpcoes[TOTAL_OPCOES] = {"JOGAR", "COMO JOGAR", "SAIR"};

static Rectangle RetanguloOpcao(int indice){// Area (na tela) do botao da opcao. Serve para desenhar e para o mouse

    return (Rectangle){
        (LARGURA_TELA - OPCAO_LARGURA) / 2.0f,
        OPCAO_TOPO + (float)indice * (OPCAO_ALTURA + OPCAO_ESPACO),
        OPCAO_LARGURA,
        OPCAO_ALTURA
    };
}

static void TextoCentralizado(const char *texto, int y, int tamanho, Color cor){// Desenha o texto no meio da tela na horizontal

    DrawText(texto, (LARGURA_TELA - MeasureText(texto, tamanho)) / 2, y, tamanho, cor);
}

void MenuInicializar(Menu *menu){
    if (menu == NULL){
         return;
    }
    menu->selecionado = 0;

    menu->mostrandoControles = false;
}

AcaoMenu MenuAtualizar(Menu *menu){
    if (menu == NULL){
         return MENU_NENHUMA;
    }

    if (menu->mostrandoControles){// Com o painel aberto, qualquer tecla de confirmacao (ou clique) fecha

        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            menu->mostrandoControles = false;
        }
        return MENU_NENHUMA;
    }

    if (IsKeyPressed(KEY_ESCAPE)){
        return MENU_SAIR;// ESC no menu principal fecha o jogo
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){
        menu->selecionado = (menu->selecionado + 1) % TOTAL_OPCOES;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
        menu->selecionado = (menu->selecionado + TOTAL_OPCOES - 1) % TOTAL_OPCOES;
    }

    Vector2 mouse = GetMousePosition();

    Vector2 movimento = GetMouseDelta();

    if (movimento.x != 0.0f || movimento.y != 0.0f){// So troca a opcao se o mouse se mexeu; assim ele nao "briga" com o teclado

        for (int i = 0; i < TOTAL_OPCOES; i++){
            if (CheckCollisionPointRec(mouse, RetanguloOpcao(i))){
                menu->selecionado = i;
            }
        }
    }

    bool clicou = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, RetanguloOpcao(menu->selecionado));

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || clicou){

        if (menu->selecionado == 0){
            return MENU_JOGAR;
        }
        else if (menu->selecionado == 1){
            menu->mostrandoControles = true;
        }
        else{
            return MENU_SAIR;
        }
    }
    return MENU_NENHUMA;
}

static void DesenharControles(void){// Painel com os controles do jogo, por cima do menu

    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){ 0, 0, 0, 170 });

    Rectangle painel = {(LARGURA_TELA - 640.0f) / 2.0f, 120.0f, 640.0f, 400.0f};

    DrawRectangleRec(painel, (Color){ 10, 15, 35, 245 });

    DrawRectangleLinesEx(painel, 2.0f, (Color){ 70, 90, 140, 255 });

    TextoCentralizado("COMO JOGAR", 142, 30, GOLD);

    const char *teclas[] = {"A / D ou setas", "ESPACO", "P", "ESC", "ENTER"};

    const char *acoes[] = {
        "Mover o Homem-Aranha",
        "Atirar teia (segure para atirar sem parar)",
        "Pausar / continuar",
        "Voltar ao menu",
        "Jogar de novo (no fim da partida)"
    };

    for (int i = 0; i < 5; i++){
        int y = 205 + i * 42;

        DrawText(teclas[i], (int)painel.x + 40, y, 20, SKYBLUE);

        DrawText(acoes[i], (int)painel.x + 230, y, 18, RAYWHITE);
    }

    TextoCentralizado("Derrote ninjas e guardas, depois os chefes: Escorpiao, Hulk e Jean Grey.", 430, 16, LIGHTGRAY);

    TextoCentralizado("Pressione ENTER ou ESC para voltar", 480, 18, GOLD);
}

void MenuDesenhar(const Menu *menu, int recorde, float rolagem){
    if (menu == NULL){
         return;
    }

    float tempo = (float)GetTime();

    ClearBackground((Color){ 8, 12, 29, 255 });

    SpritesDesenharFundo(1, rolagem);// Se o PNG nao carregou, fica so o azul-escuro de fundo

    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){ 0, 0, 0, 120 });// Escurece o fundo para o texto ficar legivel

    // Personagens nos cantos: o guarda mira o Homem-Aranha, que balanca de leve
    SpritesDesenhar(SPR_GUARDA, 150.0f, ALTURA_TELA - 40.0f - 105.0f, 210.0f, 0.0f, WHITE);

    float balanco = sinf(tempo * 2.0f) * 6.0f;

    SpritesDesenhar(SPR_JOGADOR_PARADO, LARGURA_TELA - 130.0f, ALTURA_TELA - 40.0f + balanco, 190.0f, 0.0f, WHITE);

    // Titulo com sombra
    TextoCentralizado("HOMEM-ARANHA", 73, 64, (Color){ 0, 0, 0, 200 });

    TextoCentralizado("HOMEM-ARANHA", 70, 64, RAYWHITE);

    TextoCentralizado("UM NOVO DIA", 143, 32, (Color){ 0, 0, 0, 200 });

    TextoCentralizado("UM NOVO DIA", 140, 32, (Color){ 220, 40, 50, 255 });

    // Botoes
    for (int i = 0; i < TOTAL_OPCOES; i++){
        Rectangle r = RetanguloOpcao(i);

        bool ativo = (i == menu->selecionado);

        DrawRectangleRec(r, ativo ? (Color){ 190, 30, 45, 235 } : (Color){ 10, 15, 35, 200 });

        DrawRectangleLinesEx(r, 2.0f, ativo ? RAYWHITE : (Color){ 70, 90, 140, 255 });

        int tamanho = 24;

        int larguraTexto = MeasureText(textosOpcoes[i], tamanho);

        DrawText(textosOpcoes[i], (int)(r.x + (r.width - (float)larguraTexto) / 2.0f), (int)(r.y + (r.height - (float)tamanho) / 2.0f), tamanho, ativo ? RAYWHITE : LIGHTGRAY);

        if (ativo){// Setas piscando dos dois lados da opcao selecionada
            int vai = (int)(sinf(tempo * 6.0f) * 4.0f);

            DrawText(">", (int)r.x - 28 + vai, (int)(r.y + 10.0f), 26, GOLD);

            DrawText("<", (int)(r.x + r.width) + 14 - vai, (int)(r.y + 10.0f), 26, GOLD);
        }
    }

    TextoCentralizado(TextFormat("RECORDE: %d", recorde), 505, 22, GOLD);

    TextoCentralizado("W/S ou setas: escolher   |   ENTER: confirmar   |   ESC: sair", ALTURA_TELA - 28, 15, LIGHTGRAY);

    if (menu->mostrandoControles){
        DesenharControles();
    }
}
