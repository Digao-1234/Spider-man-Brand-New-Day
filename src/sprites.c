#include "sprites.h"

#include "config.h"

#include <math.h>

typedef struct Sprite{
    const char *arquivo; // Caminho do PNG (relativo a pasta raiz do projeto)

    Rectangle recorte; // Area util do PNG. Os arquivos tem muita margem transparente; sem recortar, o desenho sairia pequeno

    float ancoraX; // Ponto do desenho (0 a 1) que fica exatamente sobre a posicao pedida: 0.5 = meio

    float ancoraY; // 0 = topo, 0.5 = meio, 1 = pes

    Texture2D textura;

    bool carregado;
} Sprite;


static Sprite sprites[SPR_TOTAL] = {// Inicializa a tabela de sprites com os dados de cada sprite.
    [SPR_JOGADOR_PARADO] = {"assets/sprites/jogador_parado.png", {  28.0f, 181.0f, 469.0f,  397.0f}, 0.70f, 1.0f},
    [SPR_JOGADOR_ESQ]    = {"assets/sprites/jogador_esq.png",    { 196.0f, 174.0f, 212.0f,  394.0f}, 0.50f, 1.0f},
    [SPR_JOGADOR_DIR]    = {"assets/sprites/jogador_dir.png",    { 201.0f, 174.0f, 212.0f,  394.0f}, 0.50f, 1.0f},
    [SPR_INIMIGO]        = {"assets/sprites/inimigo.png",        { 290.0f,  67.0f, 446.0f,  974.0f}, 0.50f, 0.5f},
    [SPR_BOSS1]          = {"assets/sprites/boss_1.png",         {   0.0f,   1.0f, 267.0f,  372.0f}, 0.50f, 0.5f},
    [SPR_BOSS2]          = {"assets/sprites/boss_2.png",         {   1.0f,   1.0f,  98.0f,  158.0f}, 0.50f, 0.5f},
    [SPR_BOSS3]          = {"assets/sprites/Boss3.png",          {  27.0f,   0.0f, 285.0f,  628.0f}, 0.50f, 0.5f},
    [SPR_TEIA]           = {"assets/sprites/projetil_jogador.png",{ 157.0f, 127.0f, 461.0f, 1131.0f}, 0.50f, 0.5f},
    [SPR_TIRO_INIMIGO]   = {"assets/sprites/projetil_inimigo.png",{ 397.0f,  74.0f, 614.0f,  606.0f}, 0.50f, 0.5f},
    [SPR_TIRO_BOSS1]     = {"assets/sprites/projetil_Boss_1.png",{ 184.0f,  98.0f, 571.0f, 1551.0f}, 0.50f, 0.5f},
    [SPR_TIRO_BOSS2]     = {"assets/sprites/projetil_Boss2.png", { 133.0f,  49.0f, 455.0f,  777.0f}, 0.50f, 0.5f},
    [SPR_TIRO_BOSS3]     = {"assets/sprites/projetil_Boss3.png", { 136.0f,  64.0f, 676.0f, 1585.0f}, 0.50f, 0.5f},
    // As 3 poses do guarda usam o MESMO recorte: assim o corpo nao "pula" quando a pose muda.
    // ancoraX 0.36 = centro do corpo (a arma aponta para a direita e nao entra na conta)
    [SPR_GUARDA]         = {"assets/sprites/guarda_ctu.png",              { 359.0f, 145.0f, 655.0f, 1055.0f}, 0.36f, 0.5f},
    [SPR_GUARDA_MIRANDO] = {"assets/sprites/guarda_ctu_apontando.png",    { 359.0f, 145.0f, 655.0f, 1055.0f}, 0.36f, 0.5f},
    [SPR_GUARDA_FRENTE]  = {"assets/sprites/guarda_ctu_frente.png",       { 359.0f, 145.0f, 655.0f, 1055.0f}, 0.36f, 0.5f},
    [SPR_TIRO_GUARDA]    = {"assets/sprites/projetil_gun_melhorado.png",  {   6.0f,   5.0f, 159.0f,  337.0f}, 0.50f, 0.5f}
};

#define TOTAL_FUNDOS 3

static const char *arquivosFundo[TOTAL_FUNDOS] = {// Um fundo por fase (fase 1, 2 e 3)
    "assets/sprites/Fase_1.png",
    "assets/sprites/Fase2.png",
    "assets/sprites/Fase_3.png"
};

static Texture2D fundos[TOTAL_FUNDOS];

static bool fundoCarregado[TOTAL_FUNDOS];


void SpritesCarregar(void){// Carrega todas as texturas. Chamar UMA vez, depois de InitWindow()

    for (int i = 0; i < SPR_TOTAL; i++){// Percorre todos os sprites da tabela

        Sprite *s = &sprites[i];// Ponteiro para o sprite atual

        s->textura = LoadTexture(s->arquivo);// Envia o PNG para a placa de video

        s->carregado = s->textura.id != 0;// id 0 = falhou (arquivo nao encontrado ou invalido)

        if (!s->carregado){// Falha ao carregar o sprite: o jogo vai usar retangulos no lugar do sprite
            TraceLog(LOG_WARNING, "SPRITES: nao foi possivel carregar %s (o jogo vai usar retangulos)", s->arquivo);

            continue;
        }

        
        GenTextureMipmaps(&s->textura);// Gera mipmaps para a textura, melhorando a qualidade da imagem quando ela é redimensionada

        SetTextureFilter(s->textura, TEXTURE_FILTER_TRILINEAR);// Define o filtro de textura para trilinear, melhorando a qualidade da imagem quando ela é redimensionada
    }

    for (int i = 0; i < TOTAL_FUNDOS; i++){// Carrega os fundos das fases

        fundos[i] = LoadTexture(arquivosFundo[i]);

        fundoCarregado[i] = fundos[i].id != 0;

        if (!fundoCarregado[i]){// Sem o PNG, o jogo desenha o fundo de estrelas de antes
            TraceLog(LOG_WARNING, "SPRITES: nao foi possivel carregar %s (o jogo vai usar o fundo simples)", arquivosFundo[i]);

            continue;
        }

        SetTextureFilter(fundos[i], TEXTURE_FILTER_BILINEAR);
    }
}

void SpritesDescarregar(void){// Libera as texturas. Chamar antes de CloseWindow()

    for (int i = 0; i < SPR_TOTAL; i++){// Percorre todos os sprites da tabela

        if (sprites[i].carregado){// Se a textura foi carregada, libera a memória da placa de vídeo

            UnloadTexture(sprites[i].textura);// Libera a textura da placa de video

            sprites[i].carregado = false;// Marca o sprite como não carregado
        }
    }

    for (int i = 0; i < TOTAL_FUNDOS; i++){// Libera os fundos das fases

        if (fundoCarregado[i]){
            UnloadTexture(fundos[i]);

            fundoCarregado[i] = false;
        }
    }
}

bool SpritesDesenhar(SpriteId id, float x, float y, float altura, float rotacao, Color tint){// Desenha o sprite especificado na posição (x, y) com a altura e rotação especificadas, aplicando a cor de tintura.

    if ((int)id < 0 || (int)id >= SPR_TOTAL || altura <= 0.0f || !sprites[id].carregado){// Verifica se o id do sprite é válido, se a altura é positiva e se o sprite foi carregado
        return false;// Sem textura: quem chamou desenha o retangulo antigo
    }

    const Sprite *s = &sprites[id];// Ponteiro para o sprite atual

    float largura = altura * s->recorte.width / s->recorte.height;// Mantem a proporcao do desenho original

    Rectangle destino = {x, y, largura, altura};// Com origem diferente de zero, x e y indicam onde a ANCORA vai parar na tela

    Vector2 origem = {largura * s->ancoraX, altura * s->ancoraY};// Ponto de ancora dentro do destino (tambem e o centro da rotacao)

    DrawTexturePro(s->textura, s->recorte, destino, origem, rotacao, tint);// Desenha o sprite na tela, com recorte, destino, origem, rotacao e cor de tintura especificados

    return true;//o sprite foi desenhado
}

bool SpritesDesenharFundo(int fase, float rolagem){// Desenha o fundo da fase ocupando a tela toda, rolando para baixo (a "camera" sobe)

    int indice = fase - 1;

    if (indice < 0 || indice >= TOTAL_FUNDOS || !fundoCarregado[indice]){
        return false;// Fase invalida ou PNG nao carregado: quem chamou desenha outro fundo
    }

    const Texture2D textura = fundos[indice];

    float escala = (float)LARGURA_TELA / (float)textura.width;// Ajusta a largura da imagem a largura da tela

    float altura = (float)textura.height * escala;// Altura da imagem depois de ajustada

    float ciclo = altura * 2.0f;// A imagem e repetida de cabeca para baixo (espelhada) a cada volta, entao a emenda nunca aparece

    float deslocamento = fmodf(rolagem, ciclo);

    if (deslocamento < 0.0f){
        deslocamento += ciclo;
    }

    Color tint = (Color){ 170, 170, 170, 255 };// Escurece um pouco o fundo para os sprites e o HUD ficarem em destaque

    for (int n = -2; n <= 0; n++){// So essas 3 posicoes podem aparecer na tela, pois a altura da imagem e maior que a da tela

        float topo = deslocamento + (float)n * altura;

        if (topo >= (float)ALTURA_TELA || topo + altura <= 0.0f){
            continue;// Esse pedaco esta fora da tela
        }

        Rectangle origem = {0.0f, 0.0f, (float)textura.width, (float)textura.height};

        if (((n % 2) + 2) % 2 == 1){
            origem.height = -origem.height;// Altura negativa = desenha espelhado na vertical
        }

        DrawTexturePro(textura, origem, (Rectangle){0.0f, topo, (float)LARGURA_TELA, altura}, (Vector2){0.0f, 0.0f}, 0.0f, tint);
    }

    return true;
}
