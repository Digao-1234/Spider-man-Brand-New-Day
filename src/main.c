#include "raylib.h"

#include "jogador.h"

#include "inimigos.h"

#include "projeteis.h"

#include "fases.h"

#include "arquivos.h"

#include "config.h"

#include "sprites.h"

#include <stdbool.h>

#include <time.h>

typedef struct Partida{
    Jogador jogador;

    ListaInimigos inimigos;

    ListaProjeteis projeteis;

    Fases fases;

    EstadoJogo estado;

    int pontuacao;

    int recorde;

    bool pausado;
} Partida;

static void PartidaInicializar(Partida *partida){
    // Inicializa a partida, configurando o jogador, inimigos, projéteis, fases e estado do jogo

    JogadorInicializar(&partida->jogador);// Inicializa o jogador com suas propriedades iniciais

    InimigosInicializar(&partida->inimigos);
    // Inicializa a lista de inimigos, configurando a formação e propriedades iniciais

    ProjeteisInicializar(&partida->projeteis);
    // Inicializa a lista de projéteis, preparando-a para armazenar os projéteis disparados durante o jogo

    FasesInicializar(&partida->fases, &partida->inimigos, &partida->projeteis);
    // Inicializa as fases do jogo, configurando a fase atual, número de ondas e o chefe da fase

    partida->estado = JOGO_EM_ANDAMENTO;// Define o estado inicial do jogo como "em andamento"

    partida->pontuacao = 0;// Inicializa a pontuação do jogador como 0

    partida->recorde = ArquivosLerRecorde();// Lê o recorde salvo no arquivo, ou 0 se não houver recorde salvo

    partida->pausado = false;
    // Inicializa o estado de pausa do jogo como falso, permitindo que o jogo seja jogado normalmente
}

static void PartidaReiniciar(Partida *partida){
    //Reinicia a partida, limpando as listas de inimigos e projéteis, reinicializando o jogador e as fases, e redefinindo o estado do jogo, pontuação e recorde

    InimigosLimpar(&partida->inimigos);// Limpa a lista de inimigos, removendo todos os elementos e liberando a memória

    ProjeteisLimpar(&partida->projeteis);// Limpa a lista de projéteis, liberando a memória

    JogadorInicializar(&partida->jogador);// Reinicializa o jogador com suas propriedades iniciais

    FasesReiniciar(&partida->fases, &partida->inimigos, &partida->projeteis);
    // Reinicia a fase atual, configurando a fase, inimigos e projéteis para o estado inicial

    partida->estado = JOGO_EM_ANDAMENTO;// Redefine o estado do jogo como "em andamento"

    partida->pontuacao = 0;// Reinicia a pontuação do jogador para 0

    partida->recorde = ArquivosLerRecorde();
    // Lê novamente o recorde salvo no arquivo, garantindo que o recorde atual seja atualizado corretamente

    partida->pausado = false;// Redefine o estado de pausa do jogo como falso, permitindo que o jogo seja jogado normalmente
}

static void SalvarRecordeSeNecessario(Partida *partida){
    // Salva o recorde atual se a pontuação do jogador for maior que o recorde salvo, garantindo que o recorde seja atualizado corretamente

    ArquivosSalvarRecordeSeMaior(partida->pontuacao);// Salva a pontuação no arquivo se ela for maior que o recorde atual

    if (partida->pontuacao > partida->recorde){
        partida->recorde = partida->pontuacao;
        // Atualiza o recorde atual com a pontuação do jogador, garantindo que o recorde seja refletido corretamente na partida
    }
}

static void PartidaAtualizar(Partida *partida, float dt){
    //Atualiza o estado da partida, incluindo o jogador, inimigos, projéteis, fases e verificando condições de vitória ou derrota

    if (partida->estado != JOGO_EM_ANDAMENTO){
        // Verifica se o jogo não está em andamento, indicando que houve vitória ou derrota

        if (IsKeyPressed(KEY_ENTER)){// Verifica se a tecla ENTER foi pressionada para reiniciar a partida
            PartidaReiniciar(partida);
            // Reinicia a partida, limpando as listas de inimigos e projéteis, reinicializando o jogador e as fases, e redefinindo o estado do jogo, pontuação e recorde
        }
        return;// Retorna sem atualizar o estado da partida, pois o jogo não está em andamento
    }

    if (IsKeyPressed(KEY_P)){// Verifica se a tecla P foi pressionada para pausar ou retomar o jogo

        partida->pausado = !partida->pausado;
        // Alterna o estado de pausa do jogo, permitindo que o jogador pause ou retome a partida
    }
    if (partida->pausado){
        // Verifica se o jogo está pausado, indicando que as atualizações do estado da partida devem ser suspensas
        
        return;// Retorna sem atualizar o estado da partida, pois o jogo está pausado
    }
    if (dt > 0.05f){// Verifica se o tempo delta (dt) é maior que 0.05 segundos, indicando que houve uma pausa ou atraso na atualização do jogo

        dt = 0.05f;
        // Limita o tempo delta para evitar atualizações muito grandes, evitando problemas de desempenho
    }

    JogadorAtualizar(&partida->jogador, &partida->projeteis, dt, true);
    // Atualiza o estado do jogador, incluindo sua posição, disparos e invulnerabilidade, com base no tempo delta (dt) e permitindo que o jogador jogue normalmente

    InimigosAtualizar(&partida->inimigos, &partida->projeteis, dt, partida->fases.faseAtual, partida->fases.ondaFaseUm);
    // Atualiza o estado dos inimigos, incluindo sua posição, disparos e comportamento, com base no tempo delta (dt), na fase atual e na onda da fase 1

    ProjeteisAtualizar(&partida->projeteis, dt);
    // Atualiza a posição dos projéteis na lista, movendo-os de acordo com sua velocidade e direção, com base no tempo delta (dt)

    ProjeteisResolverColisoes(&partida->projeteis, &partida->inimigos, &partida->fases, &partida->jogador, &partida->pontuacao);
    // Verifica e resolve as colisões entre projéteis, inimigos, fases e o jogador, atualizando a pontuação do jogador com base nas colisões ocorridas

    if (partida->jogador.vidas <= 0 || InimigosAtingiramBase(&partida->inimigos, LINHA_BASE)){
        // Verifica se o jogador perdeu todas as vidas ou se algum inimigo atingiu a base, indicando derrota

        partida->estado = JOGO_DERROTA;// Define o estado do jogo como derrota, encerrando a partida
    } 
    else {
        FasesAtualizar(&partida->fases, &partida->inimigos, &partida->projeteis, dt, &partida->estado);
        // Atualiza o estado do jogo com base na fase atual, inimigos e projéteis, verificando condições de vitória ou derrota e atualizando o estado do jogo de acordo
    }

    if (partida->estado != JOGO_EM_ANDAMENTO){
        // Verifica se o jogo não está mais em andamento, indicando que houve vitória ou derrota

        SalvarRecordeSeNecessario(partida);
        // Salva o recorde atual se a pontuação do jogador for maior que o recorde salvo, garantindo que o recorde seja atualizado corretamente
    }
}

static void PartidaDesenhar(const Partida *partida){
    // Desenha os elementos visuais da partida, incluindo o fundo, informações do jogo, inimigos, fases, projéteis e jogador, além de mensagens de pausa e fim de jogo

    ClearBackground((Color){ 8, 12, 29, 255 });
    // Limpa a tela com uma cor de fundo específica, preparando-a para desenhar os elementos visuais da partida

    
    // Cenario da fase atual (SPR_FUNDO_FASE1..3). Se o PNG nao carregou, volta ao fundo de estrelas.
    int fundo = partida->fases.faseAtual;

    bool temFundo = fundo >= 1 && fundo <= 3 &&
                    SpritesDesenhar((SpriteId)(SPR_FUNDO_FASE1 + (fundo - 1)), 0.0f, 0.0f, (float)ALTURA_TELA, 0.0f, WHITE);

    if (temFundo){
        // Faixas escuras suaves no topo e na base: o texto do HUD continua legivel sobre o cenario
        DrawRectangleGradientV(0, 0, LARGURA_TELA, 120, (Color){ 0, 0, 0, 170 }, (Color){ 0, 0, 0, 0 });

        DrawRectangleGradientV(0, ALTURA_TELA - 60, LARGURA_TELA, 60, (Color){ 0, 0, 0, 0 }, (Color){ 0, 0, 0, 170 });
    }

    for (int i = 0; i < 70 && !temFundo; i++){
        // Desenha 70 pixels aleatórios no fundo da tela, criando um efeito visual de estrelas ou partículas no espaço
        int x = (i * 137 + 29) % LARGURA_TELA;

        int y = (i * 83 + 17) % ALTURA_TELA;

        DrawPixel(x, y, (Color){ 72, 85, 125, 255 });// Desenha um pixel na posição (x, y) com uma cor específica, representando uma estrela ou partícula no fundo da tela
    }

    DrawText("HOMEM-ARANHA: UM NOVO DIA", 20, 16, 23, RAYWHITE);
    // Desenha o título do jogo na tela, indicando o nome do jogo

    DrawText(TextFormat("FASE: %d", partida->fases.faseAtual), 20, 48, 18, SKYBLUE);
    // Desenha a fase atual do jogo na tela, informando ao jogador em qual fase ele está jogando

    DrawText(TextFormat("PONTOS: %d", partida->pontuacao), 20, 73, 18, RAYWHITE);
    // Desenha a pontuação do jogador na tela

    DrawText(TextFormat("RECORDE: %d", partida->recorde), 20, 98, 18, GOLD);
    // Desenha o recorde do jogador na tela

    DrawText(TextFormat("VIDAS: %d", partida->jogador.vidas), 820, 18, 20, RAYWHITE);
    // Desenha o número de vidas restantes do jogador na tela, informando ao jogador quantas vidas ele ainda possui

    DrawLine(12, (int)LINHA_BASE, LARGURA_TELA - 12, (int)LINHA_BASE, (Color){ 45, 65, 100, 255 });
    // Desenha uma linha horizontal na tela representando a base que os inimigos não devem atingir, indicando ao jogador a área que ele deve proteger

    InimigosDesenhar(&partida->inimigos);
    // Desenha os inimigos na tela, incluindo suas posições, formações e efeitos visuais, permitindo que o jogador visualize os inimigos durante a partida

    FasesDesenhar(&partida->fases);
    //Desenha os elementos visuais relacionados à fase atual, incluindo o chefe da fase, efeitos especiais e informações adicionais, proporcionando uma experiência visual imersiva para o jogador

    ProjeteisDesenhar(&partida->projeteis);
    // Desenha os projéteis na tela, incluindo suas posições, direções e efeitos visuais, permitindo que o jogador visualize os disparos durante a partida

    JogadorDesenhar(&partida->jogador);
    // Desenha o jogador na tela, incluindo sua posição, animações e efeitos visuais, permitindo que o jogador visualize seu personagem durante a partida

    if (partida->fases.faseAtual == 1 && !partida->fases.chefe.ativo && InimigosContar(&partida->inimigos) > 0){
        DrawText(TextFormat("ONDA %d/2", partida->fases.ondaFaseUm), 805, 52, 18, LIGHTGRAY);
        //Desenha a informação da onda atual da fase 1 na tela, informando ao jogador em qual onda ele está jogando e quantas ondas ainda restam para completar a fase
    }

    if (partida->fases.faseAtual == 3 && !partida->fases.chefe.ativo && InimigosContar(&partida->inimigos) > 0){
        DrawText("ONDA 1/1", 805, 52, 18, LIGHTGRAY);// Fase 3: uma unica onda de ninjas antes do chefe final
    }

    DrawText("Mover: A/D ou setas   |   Disparar: ESPACO   |   Pausar: P   |   Sair: ESC", 20, ALTURA_TELA - 28, 15, LIGHTGRAY);
    // Desenha as instruções de controle do jogo na tela, informando ao jogador como mover, disparar, pausar e sair do jogo

    if (partida->pausado && partida->estado == JOGO_EM_ANDAMENTO){
        DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){ 0, 0, 0, 170 });
        // Desenha um retângulo semitransparente na tela, cobrindo toda a área do jogo, para indicar que o jogo está pausado e criar um efeito visual de pausa

        DrawText("PAUSADO", LARGURA_TELA / 2 - 75, 275, 34, RAYWHITE);
        // Desenha a mensagem "PAUSADO" na tela, indicando ao jogador que o jogo está atualmente pausado

        DrawText("Pressione P para continuar", LARGURA_TELA / 2 - 130, 320, 18, LIGHTGRAY);
        // Desenha a instrução para retomar o jogo na tela, informando ao jogador que ele pode pressionar a tecla P para continuar jogando
    }

    if (partida->estado == JOGO_VITORIA || partida->estado == JOGO_DERROTA){
        DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){ 0, 0, 0, 195 });
        // Desenha um retângulo semitransparente na tela, cobrindo toda a área do jogo, para indicar que o jogo terminou e criar um efeito visual de fim de jogo

        if (partida->estado == JOGO_VITORIA){
            DrawText("VOCE VENCEU!", LARGURA_TELA / 2 - 130, 230, 36, GREEN);
            // Desenha a mensagem "VOCE VENCEU!" na tela, indicando ao jogador que ele venceu o jogo

            DrawText("Nova York esta salva!", LARGURA_TELA / 2 - 118, 279, 20, RAYWHITE);
            // Desenha a mensagem "Nova York esta salva!" na tela, indicando ao jogador que a cidade foi salva
        } 
        else {
            DrawText("FIM DE JOGO", LARGURA_TELA / 2 - 123, 230, 36, RED);
            // Desenha a mensagem "FIM DE JOGO" na tela, indicando ao jogador que o jogo terminou
        }
        DrawText(TextFormat("Pontuacao: %d", partida->pontuacao), LARGURA_TELA / 2 - 76, 330, 22, RAYWHITE);
        // Desenha a pontuação final do jogador na tela

        DrawText("Pressione ENTER para jogar novamente", LARGURA_TELA / 2 - 190, 380, 18, LIGHTGRAY);
        // Desenha a instrução para reiniciar o jogo na tela, informando ao jogador que ele pode pressionar a tecla ENTER para jogar novamente
    }
}

int main(void){
    SetConfigFlags(FLAG_VSYNC_HINT);
    // Define a configuração de sincronização vertical para o jogo, garantindo que a taxa de atualização da tela seja sincronizada com a taxa de quadros do jogo

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Homem-Aranha: Um Novo Dia");
    // Inicializa a janela do jogo com as dimensões especificadas e o título "Homem-Aranha: Um Novo Dia"

    SetTargetFPS(60);
    // Define a taxa de quadros alvo para o jogo como 60 quadros por segundo

    SetRandomSeed((unsigned int)time(NULL));
    // Define a semente para a geração de números aleatórios com base no tempo atual

    SpritesCarregar();// Carrega os PNGs de assets/sprites (precisa vir depois de InitWindow)

    Partida partida = {0};
    // Inicializa a estrutura da partida com valores padrão, preparando-a para armazenar o estado do jogo, jogador, inimigos, projéteis, fases e pontuação

    PartidaInicializar(&partida);// Inicializa a partida, configurando o jogador, inimigos, projéteis, fases e estado do jogo

    while (!WindowShouldClose()){// Loop principal do jogo, que continua enquanto a janela não for fechada

        float dt = GetFrameTime();
        // Obtém o tempo delta (dt) desde o último quadro, representando o tempo decorrido entre as atualizações do jogo

        PartidaAtualizar(&partida, dt);
        // Atualiza o estado da partida, incluindo o jogador, inimigos, projéteis, fases e verificando condições de vitória ou derrota com base no tempo delta (dt)

        BeginDrawing();// Inicia o processo de desenho na tela, preparando-a para desenhar os elementos visuais da partida

        PartidaDesenhar(&partida);
        // Desenha os elementos visuais da partida, incluindo o fundo, informações do jogo, inimigos, fases, projéteis e jogador, além de mensagens de pausa e fim de jogo

        EndDrawing();// Finaliza o processo de desenho na tela, exibindo os elementos visuais desenhados durante o quadro atual
    }

    SalvarRecordeSeNecessario(&partida);
    // Salva o recorde atual se a pontuação do jogador for maior que o recorde salvo, garantindo que o recorde seja atualizado corretamente antes de encerrar o jogo

    InimigosLimpar(&partida.inimigos);// Limpa a lista de inimigos, removendo todos os elementos e liberando a memória

    ProjeteisLimpar(&partida.projeteis);// Limpa a lista de projéteis, liberando a memória

    SpritesDescarregar();// Libera as texturas antes de fechar a janela

    CloseWindow();// Fecha a janela do jogo, liberando os recursos associados à janela e encerrando a execução do jogo
    
    return 0;
}