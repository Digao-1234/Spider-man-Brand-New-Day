#include "sprites.h"

typedef struct Sprite{
    const char *arquivo; // Caminho do PNG (relativo a pasta raiz do projeto)

    Rectangle recorte; // Area util do PNG. Os arquivos tem muita margem transparente; sem recortar, o desenho sairia pequeno

    float ancoraX; // Ponto do desenho (0 a 1) que fica exatamente sobre a posicao pedida: 0.5 = meio

    float ancoraY; // 0 = topo, 0.5 = meio, 1 = pes

    Texture2D textura;

    bool carregado;
} Sprite;


static Sprite sprites[SPR_TOTAL] = {// Tabela de sprites: arquivo, recorte (area util do PNG) e ancora (0 a 1)
    [SPR_JOGADOR_PARADO]   = {"assets/sprites/jogador_parado.png",       {  28.0f, 181.0f, 469.0f,  397.0f}, 0.70f, 1.0f},
    [SPR_JOGADOR_ESQ]      = {"assets/sprites/jogador_esq.png",          { 196.0f, 174.0f, 212.0f,  394.0f}, 0.50f, 1.0f},
    [SPR_JOGADOR_DIR]      = {"assets/sprites/jogador_dir.png",          { 201.0f, 174.0f, 212.0f,  394.0f}, 0.50f, 1.0f},
    [SPR_NINJA]            = {"assets/sprites/inimigo.png",              { 290.0f,  67.0f, 446.0f,  974.0f}, 0.42f, 0.5f},
    [SPR_GUARDA]           = {"assets/sprites/guarda_ctu.png",           { 362.0f, 145.0f, 652.0f, 1055.0f}, 0.35f, 0.5f},
    [SPR_GUARDA_APONTANDO] = {"assets/sprites/guarda_ctu_apontando.png", { 362.0f, 145.0f, 465.0f, 1055.0f}, 0.49f, 0.5f},
    [SPR_BOSS1]            = {"assets/sprites/boss_1.png",               {  65.0f,  15.0f, 158.0f,  358.0f}, 0.50f, 0.5f},
    [SPR_BOSS2]            = {"assets/sprites/boss_2.png",               {   1.0f,   1.0f,  98.0f,  158.0f}, 0.50f, 0.5f},
    [SPR_BOSS3]            = {"assets/sprites/Boss3.png",                {  37.0f,  15.0f, 216.0f,  597.0f}, 0.50f, 0.5f},
    [SPR_TEIA]             = {"assets/sprites/projetil_jogador.png",     { 157.0f, 127.0f, 461.0f, 1131.0f}, 0.50f, 0.5f},
    [SPR_TIRO_NINJA]       = {"assets/sprites/projetil_inimigo.png",     { 397.0f,  74.0f, 614.0f,  606.0f}, 0.50f, 0.5f},
    [SPR_TIRO_GUARDA]      = {"assets/sprites/projetil_gun_melhorado.png",{   6.0f,   5.0f, 159.0f,  337.0f}, 0.50f, 0.5f},
    [SPR_TIRO_BOSS1]       = {"assets/sprites/projetil_Boss_1.png",      { 184.0f,  98.0f, 571.0f, 1551.0f}, 0.50f, 0.5f},
    [SPR_TIRO_BOSS2]       = {"assets/sprites/projetil_Boss2.png",       { 133.0f,  49.0f, 455.0f,  777.0f}, 0.50f, 0.5f},
    [SPR_TIRO_BOSS3]       = {"assets/sprites/projetil_Boss3.png",       { 136.0f,  64.0f, 676.0f, 1585.0f}, 0.50f, 0.5f},

    // Cenarios (imagens em retrato). O recorte e uma janela com a mesma proporcao da tela (960x640 = 3:2);
    
    [SPR_FUNDO_FASE1]      = {"assets/sprites/Fase_1.png",               {   0.0f, 426.0f, 1024.0f, 682.67f}, 0.0f, 0.0f},
    [SPR_FUNDO_FASE2]      = {"assets/sprites/Fase2.png",                {   0.0f, 261.0f,  941.0f, 627.33f}, 0.0f, 0.0f},
    [SPR_FUNDO_FASE3]      = {"assets/sprites/Fase_3.png",               {   0.0f, 170.0f, 1024.0f, 682.67f}, 0.0f, 0.0f}
};

void SpritesCarregar(void){// Carrega todas as texturas. Chamar UMA vez, depois de InitWindow()

    for (int i = 0; i < SPR_TOTAL; i++){// Percorre todos os sprites da tabela

        Sprite *s = &sprites[i];// Ponteiro para o sprite atual

        s->textura = LoadTexture(s->arquivo);// Envia o PNG para a placa de video

        s->carregado = s->textura.id != 0;// id 0 = falhou (arquivo nao encontrado ou invalido)

        if (!s->carregado){// Falha ao carregar o sprite: o jogo vai usar retangulos no lugar do sprite
            TraceLog(LOG_WARNING, "SPRITES: nao foi possivel carregar %s (o jogo vai usar retangulos)", s->arquivo);

            continue;
        }

        
        GenTextureMipmaps(&s->textura);
        // Gera mipmaps para a textura, melhorando a qualidade da imagem quando ela é redimensionada

        SetTextureFilter(s->textura, TEXTURE_FILTER_TRILINEAR);
        // Define o filtro de textura para trilinear, melhorando a qualidade da imagem quando ela é redimensionada
    }
}

void SpritesDescarregar(void){// Libera as texturas. Chamar antes de CloseWindow()

    for (int i = 0; i < SPR_TOTAL; i++){// Percorre todos os sprites da tabela

        if (sprites[i].carregado){// Se a textura foi carregada, libera a memória da placa de vídeo

            UnloadTexture(sprites[i].textura);// Libera a textura da placa de video

            sprites[i].carregado = false;// Marca o sprite como não carregado
        }
    }
}

bool SpritesDesenhar(SpriteId id, float x, float y, float altura, float rotacao, Color tint){
    // Desenha o sprite especificado na posição (x, y) com a altura e rotação especificadas, aplicando a cor de tintura.

    if ((int)id < 0 || (int)id >= SPR_TOTAL || altura <= 0.0f || !sprites[id].carregado){
        // Verifica se o id do sprite é válido, se a altura é positiva e se o sprite foi carregado
        return false;// Sem textura: quem chamou desenha o retangulo antigo
    }

    const Sprite *s = &sprites[id];// Ponteiro para o sprite atual

    float largura = altura * s->recorte.width / s->recorte.height;// Mantem a proporcao do desenho original

    Rectangle destino = {x, y, largura, altura};
    // Com origem diferente de zero, x e y indicam onde a ANCORA vai parar na tela

    Vector2 origem = {largura * s->ancoraX, altura * s->ancoraY};
    // Ponto de ancora dentro do destino (tambem e o centro da rotacao)

    DrawTexturePro(s->textura, s->recorte, destino, origem, rotacao, tint);
    // Desenha o sprite na tela, com recorte, destino, origem, rotacao e cor de tintura especificados

    return true;//o sprite foi desenhado
}