#include "inimigos.h"

#include "projeteis.h"

#include "config.h"

#include <stdlib.h>


void InimigosInicializar(ListaInimigos *lista){
    if (lista == NULL){
         return;// Verifica se a lista é nula antes de inicializar
    }
    lista->inicio = NULL;// Inicializa o ponteiro de início da lista de inimigos como NULL

    lista->direcao = 1;// Define a direção inicial dos inimigos como 1 (para a direita)

    lista->temporizadorDisparo = 0.8f;// Define o temporizador de disparo inicial dos inimigos como 0.8 segundos

    for (int l = 0; l < LINHAS_INIMIGOS; l++){
        for (int c = 0; c < COLUNAS_INIMIGOS; c++){
            lista->formacao[l][c] = 0;// Inicializa a formação de inimigos como vazia (0) para cada posição da matriz
        }
    }
}

void InimigosLimpar(ListaInimigos *lista){
    if (lista == NULL){
         return;// Verifica se a lista é nula antes de limpar os inimigos
    }
    Inimigo *atual = lista->inicio;// Define um ponteiro para percorrer a lista de inimigos

    while (atual != NULL){// Enquanto houver inimigos na lista, libera a memória de cada inimigo
        Inimigo *proximo = atual->proximo;// Armazena o próximo inimigo antes de liberar a memória do atual

        free(atual);

        atual = proximo;// Avança para o próximo inimigo na lista
    }
    lista->inicio = NULL;// Define o ponteiro de início da lista de inimigos como NULL após liberar todos os inimigos

    for (int l = 0; l < LINHAS_INIMIGOS; l++){
        for (int c = 0; c < COLUNAS_INIMIGOS; c++){
            lista->formacao[l][c] = 0;
        }
    }
}

void InimigosCriarOnda(ListaInimigos *lista){
    if (lista == NULL){
         return;// Verifica se a lista é nula antes de criar uma nova onda de inimigos
    }

    InimigosLimpar(lista);// Limpa a lista de inimigos antes de criar uma nova onda

    const float espacamentoX = 62.0f;// Define o espaçamento horizontal entre os inimigos na formação

    const float espacamentoY = 43.0f;// Define o espaçamento vertical entre os inimigos na formação

    const float larguraFormacao = (COLUNAS_INIMIGOS - 1) * espacamentoX + 34.0f;// Calcula a largura total da formação de inimigos com base no número de colunas e no espaçamento horizontal

    const float inicioX = (LARGURA_TELA - larguraFormacao) / 2.0f;// Calcula a posição inicial horizontal da formação de inimigos para centralizá-la na tela

    const float inicioY = 76.0f;// Define a posição inicial vertical da formação de inimigos na tela

    for (int l = 0; l < LINHAS_INIMIGOS; l++){
        for (int c = 0; c < COLUNAS_INIMIGOS; c++){
            Inimigo *novo = (Inimigo *)malloc(sizeof(Inimigo));// Aloca memória para um novo inimigo

            if (novo == NULL){
                 continue;// Verifica se a alocação de memória foi bem-sucedida antes de continuar
            }
            novo->corpo = (Rectangle){inicioX + c * espacamentoX, inicioY + l * espacamentoY, 34.0f, 25.0f};// Define o corpo do inimigo como um retângulo com base na posição inicial e no espaçamento da formação

            novo->linha = l;// Define a linha do inimigo na formação

            novo->coluna = c;// Define a coluna do inimigo na formação

            novo->vida = 1;// Define a vida inicial do inimigo como 1

            novo->proximo = lista->inicio;// Define o próximo inimigo como o início da lista atual

            lista->inicio = novo;// Atualiza o início da lista para apontar para o novo inimigo

            lista->formacao[l][c] = 1;// Marca a posição do inimigo na formação
        }
    }

    lista->direcao = 1;// Define a direção inicial dos inimigos como 1 (para a direita)

    lista->temporizadorDisparo = 0.75f;// Define o temporizador de disparo inicial dos inimigos como 0.75 segundos
}

int InimigosContar(const ListaInimigos *lista){
    int total = 0;// Inicializa o contador de inimigos ativos como 0

    if (lista == NULL){// Verifica se a lista é nula antes de contar os inimigos
         return 0;
    }
    for (const Inimigo *i = lista->inicio; i != NULL; i = i->proximo){
         total++;// Incrementa o contador para cada inimigo ativo na lista
    }
    return total;// Retorna o número total de inimigos ativos na lista
}

void InimigosRemover(ListaInimigos *lista, Inimigo **elo){
    if (lista == NULL || elo == NULL || *elo == NULL){
         return;// Verifica se a lista ou o ponteiro para o inimigo a ser removido é nulo antes de remover o inimigo
    }

    Inimigo *remover = *elo;// Define um ponteiro para o inimigo a ser removido

    if (remover->linha >= 0 && remover->linha < LINHAS_INIMIGOS && remover->coluna >= 0 && remover->coluna < COLUNAS_INIMIGOS){
        lista->formacao[remover->linha][remover->coluna] = 0;// Marca a posição do inimigo na formação como vazia (0) após a remoção
    }

    *elo = remover->proximo;// Atualiza o ponteiro para o próximo inimigo na lista, removendo o inimigo atual da lista

    free(remover);
}

bool InimigosAtingiramBase(const ListaInimigos *lista, float limiteY){
    if (lista == NULL){
         return false;// Verifica se a lista é nula antes de verificar se algum inimigo atingiu a base
    }

    for (const Inimigo *i = lista->inicio; i != NULL; i = i->proximo){// Percorre a lista de inimigos e verifica se algum inimigo atingiu a base (limiteY)
        if (i->corpo.y + i->corpo.height >= limiteY){// Verifica se a posição vertical do inimigo ultrapassou o limite da base
             return true;// Retorna true se algum inimigo atingiu a base
        }
    }

    return false;// Retorna false se nenhum inimigo atingiu a base
}

void InimigosAtualizar(ListaInimigos *lista, ListaProjeteis *projeteis, float dt, int fase, int onda){
    if (lista == NULL || projeteis == NULL || lista->inicio == NULL){
         return;// Verifica se a lista de inimigos ou a lista de projéteis é nula, ou se não há inimigos na lista antes de atualizar
    }

    float velocidade = 36.0f + (float)(fase - 1) * 9.0f + (float)(onda - 1) * 7.0f;// Calcula a velocidade de movimento dos inimigos com base na fase e na onda atual

    bool atingiuBorda = false;// Inicializa uma variável para verificar se algum inimigo atingiu a borda da tela

    for (Inimigo *i = lista->inicio; i != NULL; i = i->proximo){
        i->corpo.x += (float)lista->direcao * velocidade * dt;// Atualiza a posição horizontal do inimigo com base na direção e na velocidade

        if (i->corpo.x <= 10.0f || i->corpo.x + i->corpo.width >= LARGURA_TELA - 10.0f){
            atingiuBorda = true;// Verifica se algum inimigo atingiu a borda da tela (10 pixels de margem)
        }
    }

    if (atingiuBorda){
        lista->direcao *= -1;// Inverte a direção de movimento dos inimigos se algum deles atingiu a borda da tela

        for (Inimigo *i = lista->inicio; i != NULL; i = i->proximo){
            i->corpo.y += 13.0f;// Move os inimigos para baixo em 13 pixels ao atingir a borda da tela

            i->corpo.x += (float)lista->direcao * velocidade * dt;// Atualiza a posição horizontal do inimigo novamente após inverter a direção
        }
    }

    lista->temporizadorDisparo -= dt;// Atualiza o temporizador de disparo dos inimigos com base no tempo decorrido (dt)

    if (lista->temporizadorDisparo <= 0.0f && lista->inicio != NULL){
        int total = InimigosContar(lista);// Conta o número total de inimigos ativos na lista antes de disparar um projétil

        int indice = GetRandomValue(0, total - 1);// Gera um índice aleatório para selecionar um inimigo da lista para disparar um projétil

        Inimigo *escolhido = lista->inicio;// Define um ponteiro para percorrer a lista de inimigos e selecionar o inimigo escolhido para disparar

        while (indice > 0 && escolhido->proximo != NULL){
            escolhido = escolhido->proximo;// Avança para o próximo inimigo na lista até atingir o índice aleatório selecionado

            indice--;// Decrementa o índice até chegar ao inimigo escolhido
        }

        Rectangle disparo = {escolhido->corpo.x + escolhido->corpo.width / 2.0f - 3.0f, escolhido->corpo.y + escolhido->corpo.height, 6.0f, 13.0f};
        // Define a posição e o tamanho do projétil disparado pelo inimigo escolhido, centralizando-o horizontalmente em relação ao corpo do inimigo

        ProjeteisAdicionar(projeteis, disparo, 230.0f + fase * 12.0f, PROJETIL_INIMIGO);// Adiciona o projétil disparado pelo inimigo à lista de projéteis, com velocidade vertical baseada na fase atual

        lista->temporizadorDisparo = (float)GetRandomValue(65, 125) / 100.0f;// Reinicia o temporizador de disparo dos inimigos com um valor aleatório entre 0.65 e 1.25 segundos
    }

    (void)ALTURA_TELA;// Evita o aviso de variável não utilizada, pois ALTURA_TELA não é usada diretamente nesta função
}

void InimigosDesenhar(const ListaInimigos *lista){
    if (lista == NULL){
         return;// Verifica se a lista de inimigos é nula antes de desenhar os inimigos
    }
    for (const Inimigo *i = lista->inicio; i != NULL; i = i->proximo){
        Color cor = i->linha == 0 ? PURPLE : (i->linha == 1 ? ORANGE : GREEN);// Define a cor do inimigo com base na linha em que ele está na formação (linha 0 = roxo, linha 1 = laranja, linha 2 e 3 = verde)

        Rectangle r = i->corpo;// Define um retângulo com base no corpo do inimigo para desenhar na tela

        DrawRectangleRounded(r, 0.18f, 4, cor);// Desenha o corpo do inimigo como um retângulo arredondado com a cor definida anteriormente

        DrawRectangle((int)r.x + 5, (int)r.y + 6, 7, 5, WHITE);// Desenha os olhos do inimigo como retângulos brancos na posição relativa ao corpo do inimigo

        DrawRectangle((int)r.x + 22, (int)r.y + 6, 7, 5, WHITE);// Desenha os olhos do inimigo como retângulos brancos na posição relativa ao corpo do inimigo

        DrawRectangle((int)r.x + 7, (int)r.y + 17, 20, 3, DARKGRAY);// Desenha a boca do inimigo como um retângulo escuro na posição relativa ao corpo do inimigo
    }
}
