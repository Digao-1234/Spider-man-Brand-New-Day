#include "menu.h"

#include "raylib.h"

#include "config.h"

#include "sprites.h"

#include <math.h>

#define MENU_NUM_OPCOES 3
#define MENU_BOTAO_LARGURA 320
#define MENU_BOTAO_ALTURA 40
#define MENU_BOTAO_Y0 436   // Botoes mais abaixo para nao cobrir o Homem-Aranha do fundo. Para subir/descer todos, mude so este valor
#define MENU_BOTAO_PASSO 51

// Altura (centro das letras maiusculas, em pixels da tela) de cada texto. Medidas na imagem de referencia do menu
#define MENU_Y_TITULO 127.0f
#define MENU_Y_SUBTITULO 189.0f
#define MENU_Y_RECORDE 242.5f
#define MENU_Y_RODAPE 621.0f

static const char *textosOpcoes[MENU_NUM_OPCOES] = { "JOGAR", "COMO JOGAR", "SAIR" };

static Rectangle MenuBotao(int i){
    // Retangulo do botao i. Usado tanto para desenhar quanto para detectar o mouse, assim os dois nunca ficam desalinhados

    return (Rectangle){ (LARGURA_TELA - MENU_BOTAO_LARGURA) / 2.0f,
                        (float)(MENU_BOTAO_Y0 + i * MENU_BOTAO_PASSO),
                        (float)MENU_BOTAO_LARGURA,
                        (float)MENU_BOTAO_ALTURA };
            // Botao centralizado horizontalmente, com altura e largura fixas, e espaçamento vertical entre os botoes
}

static void DesenharTexto(const char *texto, float x, float centroY, int tamanho, bool centralizar, Color cor){
    // Desenha o texto com a fonte padrao da raylib, com o CENTRO das letras maiusculas em centroY. Se centralizar for true, x e o meio do texto; se nao, e o inicio
    // A fonte padrao e uma imagem de 10 px de altura: tamanhos multiplos de 10 (10, 20, 30, 40...) deixam os pixels das letras nitidos

    Font fonte = GetFontDefault();

    float espaco = (float)(tamanho / 10);// Mesmo espacamento do DrawText da raylib

    float topo = centroY - 0.4f * (float)tamanho;// Distancia do topo da linha ate o centro das maiusculas. Se o texto parecer alto/baixo demais, ajuste este 0.4

    if (centralizar){
        x -= MeasureTextEx(fonte, texto, (float)tamanho, espaco).x / 2.0f;
    }

    DrawTextEx(fonte, texto, (Vector2){ roundf(x), roundf(topo) }, (float)tamanho, espaco, cor);
}

static MenuAcao MenuAtivarOpcao(Menu *menu){
    // Executa o botao selecionado

    switch (menu->opcao){// Verifica qual opção do menu foi selecionada e retorna a ação correspondente para o main processar
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
    // Inicializa a estrutura do menu com valores padrão, preparando-a para exibir as opções e controlar a navegação do usuário
    menu->opcao = 0;

    menu->mostrandoControles = false;

    menu->tempo = 0.0f;
}

MenuAcao MenuAtualizar(Menu *menu, float dt){
    // Atualiza o estado do menu com base no tempo decorrido (dt) e nas entradas do usuário, retornando a ação correspondente para o main processar
    menu->tempo += dt;// Atualiza o tempo total desde que o menu foi aberto, usado para animações e efeitos visuais

    if (menu->mostrandoControles){
        // Se o menu estiver mostrando a tela de controles, qualquer confirmação volta ao menu principal

        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            // Verifica se o usuário pressionou ESC, ENTER ou clicou com o mouse para voltar ao menu principal
            menu->mostrandoControles = false;
        }
        return MENU_ACAO_NENHUMA;// Retorna que nenhuma ação específica foi selecionada, mantendo o menu em exibição
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){
        // Verifica se o usuário pressionou a tecla para baixo ou a tecla S para navegar para a próxima opção do menu
        menu->opcao = (menu->opcao + 1) % MENU_NUM_OPCOES;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
        // Verifica se o usuário pressionou a tecla para cima ou a tecla W para navegar para a opção anterior do menu
        menu->opcao = (menu->opcao + MENU_NUM_OPCOES - 1) % MENU_NUM_OPCOES;
    }

    bool clicou = false;// Variável para rastrear se o usuário clicou com o mouse em uma opção do menu

    Vector2 mouse = GetMousePosition();
    // Obtém a posição atual do mouse na tela, usada para verificar se o cursor está sobre algum botão do menu

    Vector2 movimento = GetMouseDelta();
    // Obtém o movimento do mouse desde o último quadro, usado para determinar se o cursor se moveu e evitar conflitos com a navegação por teclado

    for (int i = 0; i < MENU_NUM_OPCOES; i++){
        // Itera sobre todas as opções do menu para verificar se o mouse está sobre algum botão e atualizar a seleção de acordo
        if (!CheckCollisionPointRec(mouse, MenuBotao(i))){
            // Verifica se o ponto do mouse está dentro do retângulo do botão i, indicando que o cursor está sobre esse botão
            continue;
        }

        if (movimento.x != 0.0f || movimento.y != 0.0f){
            // Verifica se o mouse se moveu desde o último quadro, evitando que a seleção mude apenas por estar parado sobre um botão
            menu->opcao = i;
            // So troca a selecao quando o mouse se mexe, senao um mouse parado em cima de um botao "brigaria" com o teclado
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            // Verifica se o botão esquerdo do mouse foi pressionado enquanto o cursor está sobre o botão i, indicando que o usuário clicou nessa opção
            menu->opcao = i;

            clicou = true;
        }
    }

    if (IsKeyPressed(KEY_ENTER) || clicou){
        // Verifica se o usuário pressionou ENTER ou clicou com o mouse em uma opção do menu para confirmar a seleção

        return MenuAtivarOpcao(menu);
    }

    if (IsKeyPressed(KEY_ESCAPE)){// Verifica se o usuário pressionou ESC para sair do menu
        return MENU_ACAO_SAIR;
    }

    return MENU_ACAO_NENHUMA;// Retorna que nenhuma ação específica foi selecionada, mantendo o menu em exibição
}

static void MenuDesenharControles(void){
    // Desenha a tela de controles, mostrando as instrucoes para o jogador sobre como jogar o jogo
    DesenharTexto("COMO JOGAR", LARGURA_TELA / 2.0f, 105.0f, 40, true, RAYWHITE);

    DrawRectangle(100, 160, LARGURA_TELA - 200, 300, (Color){ 8, 12, 29, 235 });
    // Painel mais largo que antes: a fonte padrao em 20 px precisa de mais espaco para a linha mais longa

    DrawRectangleLines(100, 160, LARGURA_TELA - 200, 300, (Color){ 70, 90, 130, 255 });

    const char *teclas[] = { "A / D ou setas", "ESPACO", "P", "ENTER", "ESC" };

    const char *acoes[] = {
        "mover",
        "atirar teia (segure para continuar atirando)",
        "pausar / continuar",
        "jogar de novo (na tela de vitoria/derrota)",
        "voltar ao menu"
    };

    for (int i = 0; i < 5; i++){
        // Duas colunas: a tecla (azul) e o que ela faz (branco), uma linha a cada 48 pixels
        float y = 205.0f + (float)i * 48.0f;

        DesenharTexto(teclas[i], 130.0f, y, 20, false, SKYBLUE);

        DesenharTexto(acoes[i], 340.0f, y, 20, false, RAYWHITE);
    }

    DesenharTexto("ENTER ou ESC para voltar", LARGURA_TELA / 2.0f, ALTURA_TELA - 45.0f, 20, true, LIGHTGRAY);
}

void MenuDesenhar(const Menu *menu, int recorde){
    // Desenha o menu principal, incluindo o fundo, titulo, opcoes e o recorde
    ClearBackground((Color){ 8, 12, 29, 255 });
    // Limpa a tela com uma cor de fundo solida, preparando-a para desenhar os elementos do menu

    // Cenario do menu (assets/sprites/fundo_menu.png), na luz total, sem escurecer. Sem ele usa o cenario da fase 1 e, sem ele tambem, o fundo de estrelas
    // ATENCAO: o fundo_menu.png tem que ser so o cenario, sem texto desenhado nele, porque o titulo e os botoes sao desenhados aqui pelo codigo
    bool temFundo = SpritesDesenharFundo(SPR_FUNDO_MENU, (float)LARGURA_TELA, (float)ALTURA_TELA, WHITE)
                 || SpritesDesenharFundo(SPR_FUNDO_FASE1, (float)LARGURA_TELA, (float)ALTURA_TELA, WHITE);

    if (!temFundo){
        for (int i = 0; i < 70; i++){
            DrawPixel((i * 137 + 29) % LARGURA_TELA, (i * 83 + 17) % ALTURA_TELA, (Color){ 72, 85, 125, 255 });
        }
    }

    if (menu->mostrandoControles){
        DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){ 0, 0, 0, 150 });
        // Escurece o cenario so atras da tela de controles; o menu inicial fica com o cenario na luz total

        MenuDesenharControles();

        return;
    }

    float meio = LARGURA_TELA / 2.0f;

    DesenharTexto("HOMEM-ARANHA", meio, MENU_Y_TITULO, 60, true, (Color){ 220, 30, 45, 255 });

    DesenharTexto("UM NOVO DIA", meio, MENU_Y_SUBTITULO, 30, true, RAYWHITE);

    DesenharTexto(TextFormat("RECORDE: %d", recorde), meio, MENU_Y_RECORDE, 20, true, GOLD);

    for (int i = 0; i < MENU_NUM_OPCOES; i++){
        Rectangle botao = MenuBotao(i);

        bool selecionado = (i == menu->opcao);

        DrawRectangleRec(botao, selecionado ? (Color){ 200, 30, 40, 255 } : (Color){ 20, 30, 60, 255 });

        DrawRectangleLinesEx(botao, 2.0f, selecionado ? RAYWHITE : (Color){ 70, 90, 130, 255 });

        DesenharTexto(textosOpcoes[i], botao.x + botao.width / 2.0f, botao.y + botao.height / 2.0f, 20, true, selecionado ? RAYWHITE : LIGHTGRAY);
    }

    DesenharTexto("W/S ou setas: escolher   |   ENTER ou clique: confirmar   |   ESC: sair", meio, MENU_Y_RODAPE, 15, true, LIGHTGRAY);
}
