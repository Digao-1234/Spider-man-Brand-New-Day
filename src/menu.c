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
            // Botao centralizado horizontalmente, com altura e largura fixas, e espaçamento vertical entre os botoes
}

static void DesenharTextoCentralizado(const char *texto, int y, int tamanho, Color cor){
    DrawText(texto, (LARGURA_TELA - MeasureText(texto, tamanho)) / 2, y, tamanho, cor);
    // Desenha o texto centralizado horizontalmente na tela, com a posição vertical, tamanho e cor especificados
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
    // Desenha a tela de controles, mostrando as instruções para o jogador sobre como jogar o jogo
    DesenharTextoCentralizado("COMO JOGAR", 80, 44, RAYWHITE);

    DrawRectangle(180, 160, LARGURA_TELA - 360, 300, (Color){ 8, 12, 29, 210 });
    // Desenha um retângulo de fundo para a área de instruções, com cor semi-transparente para destacar o texto

    DrawRectangleLines(180, 160, LARGURA_TELA - 360, 300, (Color){ 70, 90, 130, 255 });
    // Desenha a borda do retângulo de instruções, com cor sólida para destacar a área de instruções

    const char *linhas[] = {
        "A / D ou setas    mover",
        "ESPACO            atirar teia (segure para continuar atirando)",
        "P                 pausar / continuar",
        "ENTER             jogar de novo (na tela de vitoria/derrota)",
        "ESC               voltar ao menu"
    };

    for (int i = 0; i < 5; i++){
        // Itera sobre as linhas de instruções e desenha cada uma na tela, espaçando-as verticalmente para melhor legibilidade
        DrawText(linhas[i], 210, 190 + i * 48, 18, RAYWHITE);
    }

    DesenharTextoCentralizado("ENTER ou ESC para voltar", ALTURA_TELA - 50, 18, LIGHTGRAY);
    // Desenha a instrução para voltar ao menu principal na parte inferior da tela, com cor clara para destaque
}

void MenuDesenhar(const Menu *menu, int recorde){
    // Desenha o menu principal, incluindo o fundo, título, opções e informações adicionais, proporcionando uma interface visual para o jogador interagir com o jogo
    ClearBackground((Color){ 8, 12, 29, 255 });
    // Limpa a tela com uma cor de fundo sólida, preparando-a para desenhar os elementos do menu

    bool temFundo = SpritesDesenharFundo(SPR_FUNDO_MENU, (float)LARGURA_TELA, (float)ALTURA_TELA, WHITE);
    // Tenta desenhar o sprite do fundo do menu, retornando true se o sprite foi carregado corretamente e desenhado com sucesso

    if (!temFundo){// Se o sprite do fundo do menu nao carregou, desenha o cenario da fase 1 como fundo alternativo
        temFundo = SpritesDesenharFundo(SPR_FUNDO_FASE1, (float)LARGURA_TELA, (float)ALTURA_TELA, WHITE);
    }

    if (temFundo){// Se o sprite do fundo foi desenhado, escurece a tela para que o titulo e os botoes fiquem legiveis
        DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){ 0, 0, 0, 140 });
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
