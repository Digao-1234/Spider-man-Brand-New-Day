#include "fases.h"

#include "inimigos.h"

#include "projeteis.h"

#include "config.h"

#include "sprites.h"

#include "audio.h"

#include <stddef.h>

#include <math.h>


static TipoProjetil TipoTiroDoChefe(int tipo){// Cada chefe dispara o seu proprio tipo de projetil (muda so o visual)
    if (tipo == 1){
         return PROJETIL_BOSS1;// Tiros do chefe da fase 1 (Escorpião)
    }
    else if (tipo == 2){
         return PROJETIL_BOSS2;// Tiros do miniboss (Hulk)
    }
    else{
         return PROJETIL_BOSS3;
    }
}

static void ChefeCriar(Fases *fases, int tipo){
    // Cria um chefe da fase com base no tipo especificado (1, 2 ou 3) e inicializa suas propriedades

    Chefe *c = &fases->chefe;// Define um ponteiro para o chefe da fase atual

    c->ativo = true;// Marca o chefe como ativo, indicando que ele está presente na fase

    c->tipo = tipo;// Define o tipo do chefe (1 = chefe da fase 1, 2 = miniboss, 3 = chefe final)

    c->direcao = 1;// Define a direção inicial do chefe como 1 (para a direita)

    c->intervaloDisparo = 0.85f;// Define o intervalo de disparo inicial do chefe como 0.85 segundos

    if (tipo == 1){// Se o tipo do chefe for 1 (chefe da fase 1), define suas propriedades específicas
        c->corpo = (Rectangle){390.0f, 64.0f, 180.0f, 150.0f};

        c->vidaMaxima = 35;

        c->velocidade = 110.0f;
    } 
    else if (tipo == 2){// Se o tipo do chefe for 2 (miniboss), define suas propriedades específicas
        c->corpo = (Rectangle){385.0f, 64.0f, 190.0f, 160.0f};

        c->vidaMaxima = 50;

        c->velocidade = 150.0f;
    } 
    else{// Se o tipo do chefe for 3 (chefe final), define suas propriedades específicas
        c->corpo = (Rectangle){385.0f, 64.0f, 190.0f, 170.0f};

        c->vidaMaxima = 75;

        c->velocidade = 185.0f;
    }
    c->vida = c->vidaMaxima;// Inicializa a vida atual do chefe como sua vida máxima
}

static void FaseComecar(Fases *fases, ListaInimigos *inimigos, ListaProjeteis *projeteis, int numero){
    // Inicia uma nova fase do jogo com base no número especificado (1, 2 ou 3) e reinicializa as listas de inimigos e projéteis
    
    fases->faseAtual = numero;// Define a fase atual como o número especificado

    fases->chefe.ativo = false;// Marca o chefe como inativo, indicando que ele não está presente na fase

    fases->chefe.vida = 0;// Define a vida atual do chefe como 0, indicando que ele não tem vida no momento

    fases->chefe.vidaMaxima = 0;// Define a vida máxima do chefe como 0, indicando que ele não tem vida máxima no momento

    fases->chefe.tipo = 0;// Define o tipo do chefe como 0, indicando que ele não tem um tipo específico no momento

    InimigosLimpar(inimigos);// Limpa a lista de inimigos, removendo todos os elementos e liberando a memória

    ProjeteisLimpar(projeteis);// Limpa a lista de projéteis, removendo todos os elementos e liberando a memória

    fases->aviso.ativo = false;// Uma fase nova nunca comeca com a faixa de fim de onda da fase anterior na tela

    if (numero == 1){// Se o número da fase for 1, define a onda da fase 1 como 1 e cria uma nova onda de inimigos
        fases->ondaFaseUm = 1;

        InimigosCriarOnda(inimigos, INIMIGO_GUARDA);
    } 
    else if (numero == 2){// Se o número da fase for 2, cria um chefe do tipo 2 (miniboss)
        ChefeCriar(fases, 2);
    } 
    else{// Se o número da fase for 3, cria uma onda de ninjas (o chefe final aparece quando todos forem eliminados)
        fases->ondaFaseUm = 1;// A velocidade da onda usa a fase 3 e a onda 1 (nao depende de como terminou a fase 1)

        InimigosCriarOnda(inimigos, INIMIGO_NINJA);// Onda de ninjas antes do chefe final
    }
}

// ---------- Transicao entre fases ----------
// Linha do tempo (segundos). Com origem > 0:
//   0.0 - 1.8  "FASE X CONCLUIDA!" aparece sobre a cena congelada, que escurece
//   1.8 - 2.3  escurece ate o preto total
//   2.3        (tela preta) a fase seguinte e criada por baixo
//   2.5 - 3.9  cartao "FASE Y" com o nome do chefe
//   3.9 - 4.5  o preto some, revelando a nova fase
// Na abertura do jogo (origem 0) a animacao comeca direto em 2.3.
// Os textos usam tamanhos multiplos de 10 (fonte padrao da raylib), como o menu, para os pixels ficarem nitidos.
#define TRANSICAO_TROCA 2.3f
#define TRANSICAO_FIM 4.5f

// Final do jogo (fase 3 concluida, destino 0): a animacao termina na tela de vitoria, sem corte.
//   0.0 - 0.5  a cena escurece (ate 55%) e "FASE 3 CONCLUIDA!" aparece, com confete caindo
//   0.5 - 2.6  texto e confete
//   2.6 - 3.2  texto some e a cena escurece ate 195/255, o mesmo escuro da tela de vitoria
//   3.2        o estado vira JOGO_VITORIA e o jogo mostra "VOCE VENCEU!"
#define TRANSICAO_FINAL_FIM 3.2f

// Aviso de fim de onda: uma faixa que abre no meio da tela, mostra o texto e fecha. Dura AVISO_FIM segundos.
//   0.0 - 0.3  a faixa abre          0.25 - 0.55  o texto aparece
//   ate 1.6    texto na tela         1.6 - 2.2    o texto some e a faixa fecha
// Quando acaba, a proxima onda (ou o chefe) aparece.
#define AVISO_FIM 2.2f

static float Rampa(float t, float inicio, float duracao){
    // 0 antes de "inicio", 1 depois de "inicio + duracao", e uma subida suave (smoothstep) entre os dois

    float v = (t - inicio) / duracao;

    if (v < 0.0f){
        v = 0.0f;
    }

    if (v > 1.0f){
        v = 1.0f;
    }

    return v * v * (3.0f - 2.0f * v);
}

// ======================= NOMES (para mudar um nome, edite SO aqui) =======================
// Cartao de ABERTURA da fase ("FASE 2"): 1 = escreve "Chefe: Hulk" embaixo. 0 = nao escreve, para o jogador nao saber
// de antemao se a fase tem chefe.
#define MOSTRAR_CHEFE_NO_CARTAO_DE_ABERTURA 0

// Faixa de fase CONCLUIDA ("FASE 1 CONCLUIDA!"): 1 = escreve "Escorpião derrotado!" embaixo. 0 = nao escreve.
// Isso fala do chefe que o jogador acabou de vencer, entao nao entrega nada da fase seguinte.
#define MOSTRAR_CHEFE_DERROTADO_NA_FAIXA 1

// (A barra de vida do chefe e o aviso "CUIDADO!" de fim de onda nao dependem destas chaves.)

// Os nomes dos chefes aparecem na barra de vida, nas animacoes entre fases e no aviso de fim de onda.
// Se trocar o genero de um chefe (ex.: Jean Grey e feminino), ajuste tambem a funcao ChefeDerrotado mais abaixo.
static const char *NomeDoChefe(int tipo){
    return tipo == 1 ? "Escorpião" : (tipo == 2 ? "Hulk" : "Jean Grey");
}

// Nome opcional de cada fase: aparece no cartao de abertura, entre "FASE N" e o chefe. "" = nao mostra nada.
// Exemplo: troque o "" da fase 1 por "O Corredor Verde".
static const char *NomeDaFase(int fase){
    switch (fase){
        case 1:  return "Caos Na Prisão";
        case 2:  return "O Monstro Solto";
        default: return "Ameaça Nível Omega";
    }
}
// ==========================================================================================

static const char *ChefeDerrotado(int tipo){
    // "Escorpião derrotado!", "Hulk derrotado!", "Jean Grey derrotada!" (concordancia certa)

    return TextFormat("%s %s", NomeDoChefe(tipo), tipo == 3 ? "derrotada!" : "derrotado!");
}

static void DesenharAviso(const Aviso *av){
    float t = av->tempo;

    const int cx = LARGURA_TELA / 2;

    bool chefe = av->proximoChefe > 0;

    float abre = Rampa(t, 0.0f, 0.3f) * (1.0f - Rampa(t, AVISO_FIM - 0.3f, 0.3f));// 0 = fechada, 1 = aberta

    int altura = (int)(130.0f * abre);

    if (altura < 4){
        return;
    }

    int y0 = 290 - altura / 2;// A faixa cresce e encolhe a partir do meio

    Color fundo = chefe ? (Color){ 70, 8, 16, 220 } : (Color){ 0, 0, 0, 200 };

    Color borda = chefe ? RED : GOLD;

    DrawRectangle(0, y0, LARGURA_TELA, altura, fundo);

    DrawRectangle(0, y0, LARGURA_TELA, 3, borda);

    DrawRectangle(0, y0 + altura - 3, LARGURA_TELA, 3, borda);

    float a = Rampa(t, 0.25f, 0.3f) * (1.0f - Rampa(t, AVISO_FIM - 0.6f, 0.3f));

    if (a <= 0.0f){
        return;
    }

    const char *linha1 = TextFormat("ONDA %d CONCLUIDA!", av->onda);

    DrawText(linha1, cx - MeasureText(linha1, 40) / 2, 245, 40, Fade(GOLD, a));

    if (chefe){
        float pisca = 0.65f + 0.35f * sinf(t * 12.0f);// O aviso do chefe pisca

        const char *linha2 = TextFormat("CUIDADO! %s se aproxima", NomeDoChefe(av->proximoChefe));

        DrawText(linha2, cx - MeasureText(linha2, 20) / 2, 300, 20, Fade(RAYWHITE, a * pisca));
    }
    else {
        const char *linha2 = TextFormat("Prepare-se para a onda %d", av->onda + 1);

        DrawText(linha2, cx - MeasureText(linha2, 20) / 2, 300, 20, Fade(RAYWHITE, a));
    }
}

static void AvisoIniciar(Fases *fases, ListaProjeteis *projeteis, int onda, int proximoChefe){
    Aviso *av = &fases->aviso;

    av->ativo = true;

    av->tempo = 0.0f;

    av->fase = fases->faseAtual;

    av->onda = onda;

    av->proximoChefe = proximoChefe;

    ProjeteisLimpar(projeteis);// Tiros que sobraram no ar somem, senao ficariam parados atras da faixa
}

static void DesenharConfete(float t, float alphaGeral){
    // Chuva de confete. As posicoes saem de formulas fixas (sem aleatorio), entao a animacao e sempre igual

    Color cores[5] = { GOLD, SKYBLUE, RED, GREEN, RAYWHITE };

    for (int i = 0; i < 60; i++){
        float atraso = (float)((i * 53) % 100) / 100.0f * 1.2f;// Cada pedaco comeca a cair num momento diferente

        if (t < atraso){
            continue;
        }

        float velocidade = 110.0f + (float)((i * 37) % 110);

        float y = (t - atraso) * velocidade - 20.0f;

        if (y > ALTURA_TELA){
            continue;
        }

        float x = (float)((i * 137 + 29) % LARGURA_TELA) + sinf(t * 3.0f + (float)i) * 14.0f;// Balanca de leve para os lados

        DrawRectangle((int)x, (int)y, 6, 10, Fade(cores[i % 5], alphaGeral));
    }
}

static void DesenharTransicaoFinal(float t){
    float escuro = 0.55f * Rampa(t, 0.0f, 0.5f) + (195.0f / 255.0f - 0.55f) * Rampa(t, 2.6f, 0.6f);
    // Termina em 195/255: igual ao escuro da tela de vitoria, entao a troca para ela nao "pisca"

    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLACK, escuro));

    float a = Rampa(t, 0.1f, 0.4f) * (1.0f - Rampa(t, 2.6f, 0.5f));// Aparece rapido e some quando o escuro final chega

    DesenharConfete(t, a);

    if (a > 0.0f){
        const int cx = LARGURA_TELA / 2;

        const char *linha1 = "FASE 3 CONCLUIDA!";

        DrawText(linha1, cx - MeasureText(linha1, 40) / 2, 240, 40, Fade(GOLD, a));

        if (MOSTRAR_CHEFE_DERROTADO_NA_FAIXA){
            const char *linha2 = ChefeDerrotado(3);

            DrawText(linha2, cx - MeasureText(linha2, 20) / 2, 300, 20, Fade(RAYWHITE, a));
        }

        DrawText("ENTER para pular", LARGURA_TELA - MeasureText("ENTER para pular", 20) - 20, ALTURA_TELA - 36, 20, Fade(LIGHTGRAY, a * 0.8f));
    }
}

static void TransicaoIniciar(Fases *fases, ListaProjeteis *projeteis, int origem, int destino){
    Transicao *tr = &fases->transicao;

    tr->ativa = true;

    tr->faseOrigem = origem;

    tr->faseDestino = destino;

    tr->tempo = origem == 0 ? TRANSICAO_TROCA : 0.0f;

    tr->trocou = (origem == 0);// Na abertura o cenario da fase ja esta pronto, so falta o cartao
    // (destino 0 = final do jogo: nao existe fase seguinte para criar)

    ProjeteisLimpar(projeteis);// Tiros que estavam no ar somem, senao ficariam parados na tela durante a animacao
}

bool FasesEmTransicao(const Fases *fases){
    return fases != NULL && (fases->transicao.ativa || fases->aviso.ativo);
}

void FasesDesenharTransicao(const Fases *fases){
    if (fases == NULL){
        return;
    }

    if (fases->aviso.ativo){
        DesenharAviso(&fases->aviso);// Faixa de fim de onda

        return;
    }

    if (!fases->transicao.ativa){
        return;
    }

    const Transicao *tr = &fases->transicao;

    float t = tr->tempo;

    if (tr->faseDestino == 0){
        DesenharTransicaoFinal(t);// Fim do jogo: animacao propria, que termina na tela de vitoria

        return;
    }

    float preto = 0.55f * Rampa(t, 0.0f, 0.5f) + 0.45f * Rampa(t, 1.8f, 0.5f) - 1.0f * Rampa(t, 3.9f, 0.6f);

    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLACK, preto));

    const int cx = LARGURA_TELA / 2;

    if (tr->faseOrigem > 0){
        float a = Rampa(t, 0.1f, 0.4f) * (1.0f - Rampa(t, 1.8f, 0.4f));

        if (a > 0.0f){
            const char *linha1 = TextFormat("FASE %d CONCLUIDA!", tr->faseOrigem);

            DrawText(linha1, cx - MeasureText(linha1, 40) / 2, 240, 40, Fade(GREEN, a));

            if (MOSTRAR_CHEFE_DERROTADO_NA_FAIXA){
                const char *linha2 = ChefeDerrotado(tr->faseOrigem);

                DrawText(linha2, cx - MeasureText(linha2, 20) / 2, 300, 20, Fade(RAYWHITE, a));
            }
        }
    }

    float aTitulo = Rampa(t, 2.5f, 0.4f) * (1.0f - Rampa(t, 3.9f, 0.5f));

    if (aTitulo > 0.0f){
        int sobe = (int)((1.0f - Rampa(t, 2.5f, 0.6f)) * 24.0f);// O titulo sobe 24 px enquanto aparece

        const char *titulo = TextFormat("FASE %d", tr->faseDestino);

        DrawText(titulo, cx - MeasureText(titulo, 70) / 2, 230 + sobe, 70, Fade(RAYWHITE, aTitulo));

        int larguraLinha = (int)(380.0f * Rampa(t, 2.5f, 0.7f));// Linha vermelha que cresce do centro para as pontas

        DrawRectangle(cx - larguraLinha / 2, 320, larguraLinha, 4, Fade((Color){ 220, 30, 45, 255 }, aTitulo));

        float aSub = Rampa(t, 2.8f, 0.4f) * (1.0f - Rampa(t, 3.9f, 0.5f));

        const char *nomeFase = NomeDaFase(tr->faseDestino);

        int yChefe = 340;

        int tamanhoChefe = 30;

        if (nomeFase[0] != '\0'){
            // A fase tem nome: ele ocupa a linha principal e o chefe desce para uma linha menor

            DrawText(nomeFase, cx - MeasureText(nomeFase, 30) / 2, 340, 30, Fade(RAYWHITE, aSub));

            yChefe = 385;

            tamanhoChefe = 20;
        }

        if (MOSTRAR_CHEFE_NO_CARTAO_DE_ABERTURA){
            const char *sub = TextFormat("Chefe: %s", NomeDoChefe(tr->faseDestino));

            DrawText(sub, cx - MeasureText(sub, tamanhoChefe) / 2, yChefe, tamanhoChefe, Fade(LIGHTGRAY, aSub));
        }

        DrawText("ENTER para pular", LARGURA_TELA - MeasureText("ENTER para pular", 20) - 20, ALTURA_TELA - 36, 20, Fade(LIGHTGRAY, aTitulo * 0.8f));
    }
}

void FasesInicializar(Fases *fases, ListaInimigos *inimigos, ListaProjeteis *projeteis){// Inicializa as fases do jogo, definindo a fase atual como 1, a onda da fase 1 como 1, e inicializando as listas de inimigos e projéteis
    if (fases == NULL || inimigos == NULL || projeteis == NULL){
         return;// Verifica se algum dos ponteiros é nulo antes de inicializar as fases
    }
    fases->faseAtual = 1;// Define a fase atual como 1, indicando que o jogo começa na primeira fase

    fases->ondaFaseUm = 1;// Define a onda da fase 1 como 1, indicando que a primeira onda de inimigos será criada na fase 1

    fases->chefe = (Chefe){0};

    fases->aviso = (Aviso){0};

    fases->transicao = (Transicao){0};// Sem animacao: o jogo comeca no menu, o cartao da fase 1 aparece ao clicar em JOGAR
    // Inicializa a estrutura do chefe da fase com valores padrão (zeros), indicando que não há chefe ativo no momento

    InimigosInicializar(inimigos);
    // Inicializa a lista de inimigos, definindo o ponteiro de início como NULL, a direção como 1 (para a direita), o temporizador de disparo como 0.8 segundos, e preenchendo a formação de inimigos com zeros (vazia)

    ProjeteisInicializar(projeteis);
    // Inicializa a lista de projéteis, definindo o ponteiro de início como NULL, indicando que não há projéteis ativos no momento

    InimigosCriarOnda(inimigos, INIMIGO_GUARDA);
    // Cria a primeira onda de inimigos na fase 1, preenchendo a lista de inimigos com inimigos em formação
}

void FasesReiniciar(Fases *fases, ListaInimigos *inimigos, ListaProjeteis *projeteis){
    if (fases == NULL || inimigos == NULL || projeteis == NULL){
         return;// Verifica se algum dos ponteiros é nulo antes de reiniciar as fases
    }
    FaseComecar(fases, inimigos, projeteis, 1);// Reinicia o jogo, começando novamente na fase 1, limpando as listas de inimigos e projéteis, e criando a primeira onda de inimigos

    TransicaoIniciar(fases, projeteis, 0, 1);// Abre a partida com o cartao "FASE 1"
}

void FasesAtualizar(Fases *fases, ListaInimigos *inimigos, ListaProjeteis *projeteis, float dt, EstadoJogo *estado){
    if (fases == NULL || inimigos == NULL || projeteis == NULL || estado == NULL || *estado != JOGO_EM_ANDAMENTO){
             return;// Verifica se algum dos ponteiros é nulo ou se o estado do jogo não está em andamento antes de atualizar as fases
        }

    if (fases->transicao.ativa){
        // Durante a animacao so o relogio dela anda: chefes e ondas ficam parados ate ela terminar

        Transicao *tr = &fases->transicao;

        if (tr->faseDestino == 0){
            // Final do jogo: nao ha fase para criar. Quando a animacao acaba, o jogo entra no estado de vitoria

            if (IsKeyPressed(KEY_ENTER)){
                tr->tempo = TRANSICAO_FINAL_FIM;// ENTER pula direto para o fim
            }

            tr->tempo += dt;

            if (tr->tempo >= TRANSICAO_FINAL_FIM){
                tr->ativa = false;

                *estado = JOGO_VITORIA;
            }

            return;
        }

        if (IsKeyPressed(KEY_ENTER)){
            // ENTER pula a animacao. Se ainda nao trocou de fase, troca agora

            if (!tr->trocou){
                FaseComecar(fases, inimigos, projeteis, tr->faseDestino);

                tr->trocou = true;
            }

            tr->ativa = false;

            return;
        }

        tr->tempo += dt;

        if (!tr->trocou && tr->tempo >= TRANSICAO_TROCA){
            FaseComecar(fases, inimigos, projeteis, tr->faseDestino);// A tela esta preta: o cenario novo e criado sem ninguem ver a troca

            tr->trocou = true;
        }

        if (tr->tempo >= TRANSICAO_FIM){
            tr->ativa = false;
        }

        return;
    }

    if (fases->aviso.ativo){
        // Faixa de fim de onda: so o relogio dela anda. Quando acaba, aparece o que vem a seguir

        Aviso *av = &fases->aviso;

        av->tempo += dt;

        if (av->tempo >= AVISO_FIM){
            av->ativo = false;

            if (av->proximoChefe > 0){
                ChefeCriar(fases, av->proximoChefe);
            }
            else {
                fases->ondaFaseUm = av->onda + 1;

                InimigosCriarOnda(inimigos, av->fase == 3 ? INIMIGO_NINJA : INIMIGO_GUARDA);
            }
        }

        return;
    }

    if (fases->faseAtual == 1 && !fases->chefe.ativo && fases->chefe.tipo == 0){
// Se a fase atual for 1, o chefe não estiver ativo e o tipo do chefe for 0 (não definido), verifica se todos os inimigos foram eliminados para avançar para a próxima onda ou criar o chefe da fase

        if (InimigosContar(inimigos) == 0){// Se não houver inimigos ativos na lista, verifica se é necessário avançar para a próxima onda ou criar o chefe da fase

            if (fases->ondaFaseUm == 1){// Onda 1 da fase 1 acabou: faixa de aviso e, quando ela termina, a onda 2 aparece
                AvisoIniciar(fases, projeteis, 1, 0);
            }
            else {// Onda 2 da fase 1 acabou: faixa de aviso e, quando ela termina, o chefe da fase 1 (tipo 1) aparece
                AvisoIniciar(fases, projeteis, 2, 1);
            }
        }
    }

    if (fases->faseAtual == 3 && !fases->chefe.ativo && fases->chefe.tipo == 0){
        // Fase 3: quando todos os ninjas da onda forem eliminados, o chefe final (tipo 3) aparece
        if (InimigosContar(inimigos) == 0){
            AvisoIniciar(fases, projeteis, 1, 3);// Onda de ninjas acabou: faixa de aviso e, quando ela termina, o chefe final (tipo 3) aparece
        }
    }

    //Movimento e ataques de chefes.

    if (fases->chefe.ativo){
        // Se o chefe estiver ativo, atualiza sua posição, verifica colisões com as bordas da tela e gerencia seus ataques

        Chefe *c = &fases->chefe;// Define um ponteiro para o chefe da fase atual

        c->corpo.x += (float)c->direcao * c->velocidade * dt;// Atualiza a posição horizontal do chefe com base na direção e na velocidade, multiplicando pelo delta de tempo (dt) para garantir movimento suave e consistente

        if (c->corpo.x < CenarioLimiteEsquerdo(c->tipo)){
        // Se a posição horizontal do chefe for menor que 10.0f (atingiu a borda esquerda da tela), ajusta sua posição para 10.0f e inverte a direção para a direita

            c->corpo.x = CenarioLimiteEsquerdo(c->tipo);
            // Ajusta a posição horizontal do chefe para 10.0f, garantindo que ele não ultrapasse a borda esquerda da tela

            c->direcao = 1;
            // Inverte a direção do chefe para a direita (1), permitindo que ele se mova de volta para dentro da tela
        }
        if (c->corpo.x + c->corpo.width > CenarioLimiteDireito(c->tipo)){
    // Se a posição horizontal do chefe mais sua largura for maior que LARGURA_TELA - 10.0f (atingiu a borda direita da tela), ajusta sua posição para a borda direita e inverte a direção para a esquerda

            c->corpo.x = CenarioLimiteDireito(c->tipo) - c->corpo.width;
            // Ajusta a posição horizontal do chefe para a borda direita da tela, garantindo que ele não ultrapasse a borda direita

            c->direcao = -1;
            // Inverte a direção do chefe para a esquerda (-1), permitindo que ele se mova de volta para dentro da tela
        }

        c->intervaloDisparo -= dt;
        // Reduz o intervalo de disparo do chefe com base no delta de tempo (dt), permitindo que ele dispare projéteis em intervalos regulares

        if (c->intervaloDisparo <= 0.0f){
            // Se o intervalo de disparo do chefe for menor ou igual a 0.0f, significa que ele está pronto para disparar projéteis

            if (c->tipo == 2){
                // Boss 2 (Hulk): UM unico disparo grande. O hitbox (corpo) tem o mesmo tamanho do desenho do tiro
                Rectangle tiroGrande = {c->corpo.x + c->corpo.width / 2.0f - TIRO_BOSS2_LARGURA / 2.0f, c->corpo.y + c->corpo.height, TIRO_BOSS2_LARGURA, TIRO_BOSS2_ALTURA};

                ProjeteisAdicionar(projeteis, tiroGrande, 260.0f + c->tipo * 25.0f, PROJETIL_BOSS2);
            }
            else{
            Rectangle tiro = {c->corpo.x + c->corpo.width / 2.0f - 4.0f, c->corpo.y + c->corpo.height, 8.0f, 16.0f};
            // Define a posição e o tamanho do projétil que o chefe irá disparar, centralizando-o horizontalmente em relação ao corpo do chefe e posicionando-o logo abaixo do corpo

            ProjeteisAdicionar(projeteis, tiro, 260.0f + c->tipo * 25.0f, TipoTiroDoChefe(c->tipo));
            // Adiciona o projétil disparado pelo chefe à lista de projéteis, com velocidade vertical baseada no tipo do chefe (quanto maior o tipo, maior a velocidade do projétil)

            if (c->tipo == 3){
                // Chefe final (tipo 3): dispara projéteis adicionais em posições laterais para aumentar a dificuldade do ataque

                float espalha = 130.0f;// Distancia dos tiros laterais ate o centro do chefe (a mesma de antes)

                float centroChefe = c->corpo.x + c->corpo.width / 2.0f;

                Rectangle tiroEsq = {centroChefe - espalha - 3.5f, c->corpo.y + c->corpo.height, 7.0f, 14.0f};
                // Tiro lateral esquerdo

                Rectangle tiroDir = {centroChefe + espalha - 3.5f, c->corpo.y + c->corpo.height, 7.0f, 14.0f};
                // Tiro lateral direito

                ProjeteisAdicionar(projeteis, tiroEsq, 250.0f, TipoTiroDoChefe(c->tipo));
                // Adiciona o projétil disparado pelo lado esquerdo do chefe à lista de projéteis, com velocidade vertical fixa de 250.0f

                ProjeteisAdicionar(projeteis, tiroDir, 250.0f, TipoTiroDoChefe(c->tipo));
                // Adiciona o projétil disparado pelo lado direito do chefe à lista de projéteis, com velocidade vertical fixa de 250.0f
            }
            }
            AudioTocar(c->tipo == 1 ? SOM_TIRO_BOSS1 : (c->tipo == 2 ? SOM_TIRO_BOSS2 : SOM_TIRO_BOSS3));// Um som por rajada (a Jean Grey dispara 3 projeteis de uma vez)

            c->intervaloDisparo = c->tipo == 3 ? 0.52f : 0.9f;
            // Reinicia o intervalo de disparo do chefe com base no tipo do chefe (quanto maior o tipo, menor o intervalo de disparo)
        }
    } 
    else if (fases->chefe.tipo == 1 && fases->chefe.vida == 0){// Se o tipo do chefe for 1 (chefe da fase 1) e sua vida for 0, inicia a fase 2 com o miniboss (tipo 2)
        TransicaoIniciar(fases, projeteis, 1, 2);// Escorpiao derrotado: anima a passagem para a fase 2
    } 
    else if (fases->chefe.tipo == 2 && fases->chefe.vida == 0){// Se o tipo do chefe for 2 (miniboss) e sua vida for 0, inicia a fase 3 com o chefe final (tipo 3)
        TransicaoIniciar(fases, projeteis, 2, 3);// Hulk derrotado: anima a passagem para a fase 3
    } 
    else if (fases->chefe.tipo == 3 && fases->chefe.vida == 0){// Se o tipo do chefe for 3 (chefe final) e sua vida for 0, marca o estado do jogo como vitória, indicando que o jogador venceu o jogo
        TransicaoIniciar(fases, projeteis, 3, 0);// Jean Grey derrotada: anima o final e so depois mostra a tela de vitoria
    }
}

static float EscalaVisualChefe(int tipo){
    // Fator so do DESENHO do chefe (1.0 = tamanho do corpo). O hitbox (corpo) nao muda. Ajuste aqui se ainda achar grande/pequeno

    if (tipo == 1){
        return 0.70f; // Escorpiao
    }

    if (tipo == 3){
        return 0.70f; // Jean Grey
    }

    return 1.0f; // Hulk (miniboss) continua como estava
}

void FasesDesenhar(const Fases *fases){
    if (fases == NULL || !fases->chefe.ativo){
        // Verifica se o ponteiro de fases é nulo ou se o chefe não está ativo antes de desenhar o chefe da fase
         return;
    }
    const Chefe *c = &fases->chefe;
    // Define um ponteiro constante para o chefe da fase atual, permitindo acessar suas propriedades sem modificá-las

    Color cor = c->tipo == 1 ? MAROON : (c->tipo == 2 ? PURPLE : DARKPURPLE);
    // Define a cor do chefe com base no tipo do chefe (tipo 1 = marrom, tipo 2 = roxo, tipo 3 = roxo escuro)

    SpriteId sprite = (SpriteId)(SPR_BOSS1 + (c->tipo - 1));// SPR_BOSS1, SPR_BOSS2 e SPR_BOSS3 estao em sequencia no enum

    if (!SpritesDesenhar(sprite, c->corpo.x + c->corpo.width / 2.0f, c->corpo.y + c->corpo.height / 2.0f, c->corpo.height * EscalaVisualChefe(c->tipo), 0.0f, WHITE)){

        //se o PNG nao carregou, desenha o chefe com retangulos como antes
        DrawRectangleRounded(c->corpo, 0.16f, 8, cor);// Desenha o corpo do chefe como um retângulo arredondado com a cor definida anteriormente

        DrawRectangle((int)c->corpo.x + 22, (int)c->corpo.y + 18, 35, 12, WHITE);
        // Desenha a parte frontal do chefe (como uma "boca" ou "visor") como um retângulo branco na posição relativa ao corpo do chefe

        DrawRectangle((int)(c->corpo.x + c->corpo.width - 57), (int)c->corpo.y + 18, 35, 12, WHITE);
        // Desenha a parte frontal do chefe como um retângulo branco na posição relativa ao corpo do chefe, mas no lado oposto
    }

    const char *nome = NomeDoChefe(c->tipo);// Mesmo nome das animacoes: so existe um lugar para editar (NomeDoChefe)
    // Define o nome do chefe com base no tipo do chefe (tipo 1 = "Escorpião", tipo 2 = "Hulk", tipo 3 = "Jean Grey")

    DrawText(nome, LARGURA_TELA / 2 - 60, 18, 20, RAYWHITE);
    // Desenha o nome do chefe na tela, centralizado horizontalmente e posicionado verticalmente a 18 pixels do topo da tela, com tamanho de fonte 20 e cor branca

    const float largura = 300.0f;// Define a largura da barra de vida do chefe como 300 pixels

    float fracao = c->vidaMaxima > 0 ? (float)c->vida / (float)c->vidaMaxima : 0.0f;
    // Calcula a fração da vida atual do chefe em relação à vida máxima, garantindo que não haja divisão por zero (se a vida máxima for maior que 0, calcula a fração; caso contrário, define como 0.0f)

    if (fracao < 0.0f){// Garante que a fração da vida não seja negativa, caso a vida atual do chefe seja menor que 0
         fracao = 0.0f;
    }
    DrawRectangle((int)(LARGURA_TELA / 2 - largura / 2), 43, (int)largura, 12, DARKGRAY);
    // Desenha a barra de fundo da vida do chefe como um retângulo cinza escuro, centralizado horizontalmente na tela e posicionado verticalmente a 43 pixels do topo da tela, com largura definida anteriormente e altura de 12 pixels

    DrawRectangle((int)(LARGURA_TELA / 2 - largura / 2), 43, (int)(largura * fracao), 12, RED);
    // Desenha a barra de vida atual do chefe como um retângulo vermelho, centralizado horizontalmente na tela e posicionado verticalmente a 43 pixels do topo da tela, com largura proporcional à fração da vida atual em relação à vida máxima e altura de 12 pixels

    DrawRectangleLines((int)(LARGURA_TELA / 2 - largura / 2), 43, (int)largura, 12, RAYWHITE);
    // Desenha a borda da barra de vida do chefe como um retângulo branco, centralizado horizontalmente na tela e posicionado verticalmente a 43 pixels do topo da tela, com largura definida anteriormente e altura de 12 pixels
}