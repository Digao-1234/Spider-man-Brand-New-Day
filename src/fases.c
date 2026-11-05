#include "fases.h"

#include "inimigos.h"

#include "projeteis.h"

#include "config.h"

#include <stddef.h>


static void ChefeCriar(Fases *fases, int tipo){// Cria um chefe da fase com base no tipo especificado (1, 2 ou 3) e inicializa suas propriedades
    Chefe *c = &fases->chefe;// Define um ponteiro para o chefe da fase atual

    c->ativo = true;// Marca o chefe como ativo, indicando que ele está presente na fase

    c->tipo = tipo;// Define o tipo do chefe (1 = chefe da fase 1, 2 = miniboss, 3 = chefe final)

    c->direcao = 1;// Define a direção inicial do chefe como 1 (para a direita)

    c->intervaloDisparo = 0.85f;// Define o intervalo de disparo inicial do chefe como 0.85 segundos

    if (tipo == 1){// Se o tipo do chefe for 1 (chefe da fase 1), define suas propriedades específicas
        c->corpo = (Rectangle){360.0f, 82.0f, 240.0f, 78.0f};

        c->vidaMaxima = 22;

        c->velocidade = 110.0f;
    } 
    else if (tipo == 2){// Se o tipo do chefe for 2 (miniboss), define suas propriedades específicas
        c->corpo = (Rectangle){385.0f, 80.0f, 190.0f, 72.0f};

        c->vidaMaxima = 28;

        c->velocidade = 150.0f;
    } 
    else{// Se o tipo do chefe for 3 (chefe final), define suas propriedades específicas
        c->corpo = (Rectangle){325.0f, 65.0f, 310.0f, 100.0f};

        c->vidaMaxima = 52;

        c->velocidade = 185.0f;
    }
    c->vida = c->vidaMaxima;// Inicializa a vida atual do chefe como sua vida máxima
}

static void FaseComecar(Fases *fases, ListaInimigos *inimigos, ListaProjeteis *projeteis, int numero){// Inicia uma nova fase do jogo com base no número especificado (1, 2 ou 3) e reinicializa as listas de inimigos e projéteis
    fases->faseAtual = numero;// Define a fase atual como o número especificado

    fases->chefe.ativo = false;// Marca o chefe como inativo, indicando que ele não está presente na fase

    fases->chefe.vida = 0;// Define a vida atual do chefe como 0, indicando que ele não tem vida no momento

    fases->chefe.vidaMaxima = 0;// Define a vida máxima do chefe como 0, indicando que ele não tem vida máxima no momento

    fases->chefe.tipo = 0;// Define o tipo do chefe como 0, indicando que ele não tem um tipo específico no momento

    InimigosLimpar(inimigos);// Limpa a lista de inimigos, removendo todos os elementos e liberando a memória

    ProjeteisLimpar(projeteis);// Limpa a lista de projéteis, removendo todos os elementos e liberando a memória

    if (numero == 1){// Se o número da fase for 1, define a onda da fase 1 como 1 e cria uma nova onda de inimigos
        fases->ondaFaseUm = 1;

        InimigosCriarOnda(inimigos);
    } 
    else if (numero == 2){// Se o número da fase for 2, cria um chefe do tipo 2 (miniboss)
        ChefeCriar(fases, 2);
    } 
    else{// Se o número da fase for 3, cria um chefe do tipo 3 (chefe final)
        ChefeCriar(fases, 3);
    }
}

void FasesInicializar(Fases *fases, ListaInimigos *inimigos, ListaProjeteis *projeteis){// Inicializa as fases do jogo, definindo a fase atual como 1, a onda da fase 1 como 1, e inicializando as listas de inimigos e projéteis
    if (fases == NULL || inimigos == NULL || projeteis == NULL){
         return;// Verifica se algum dos ponteiros é nulo antes de inicializar as fases
    }
    fases->faseAtual = 1;// Define a fase atual como 1, indicando que o jogo começa na primeira fase

    fases->ondaFaseUm = 1;// Define a onda da fase 1 como 1, indicando que a primeira onda de inimigos será criada na fase 1

    fases->chefe = (Chefe){0};// Inicializa a estrutura do chefe da fase com valores padrão (zeros), indicando que não há chefe ativo no momento

    InimigosInicializar(inimigos);// Inicializa a lista de inimigos, definindo o ponteiro de início como NULL, a direção como 1 (para a direita), o temporizador de disparo como 0.8 segundos, e preenchendo a formação de inimigos com zeros (vazia)

    ProjeteisInicializar(projeteis);// Inicializa a lista de projéteis, definindo o ponteiro de início como NULL, indicando que não há projéteis ativos no momento

    InimigosCriarOnda(inimigos);// Cria a primeira onda de inimigos na fase 1, preenchendo a lista de inimigos com inimigos em formação
}

void FasesReiniciar(Fases *fases, ListaInimigos *inimigos, ListaProjeteis *projeteis){
    if (fases == NULL || inimigos == NULL || projeteis == NULL){
         return;// Verifica se algum dos ponteiros é nulo antes de reiniciar as fases
    }
    FaseComecar(fases, inimigos, projeteis, 1);// Reinicia o jogo, começando novamente na fase 1, limpando as listas de inimigos e projéteis, e criando a primeira onda de inimigos
}

void FasesAtualizar(Fases *fases, ListaInimigos *inimigos, ListaProjeteis *projeteis, float dt, EstadoJogo *estado){
    if (fases == NULL || inimigos == NULL || projeteis == NULL || estado == NULL || *estado != JOGO_EM_ANDAMENTO){
             return;// Verifica se algum dos ponteiros é nulo ou se o estado do jogo não está em andamento antes de atualizar as fases
        }

    if (fases->faseAtual == 1 && !fases->chefe.ativo && fases->chefe.tipo == 0){
// Se a fase atual for 1, o chefe não estiver ativo e o tipo do chefe for 0 (não definido), verifica se todos os inimigos foram eliminados para avançar para a próxima onda ou criar o chefe da fase
        if (InimigosContar(inimigos) == 0){// Se não houver inimigos ativos na lista, verifica se é necessário avançar para a próxima onda ou criar o chefe da fase

            if (fases->ondaFaseUm == 1){// Se a onda da fase 1 for 1, define a onda da fase 1 como 2 e cria uma nova onda de inimigos
                fases->ondaFaseUm = 2;// Avança para a segunda onda da fase 1

                InimigosCriarOnda(inimigos);// Cria a segunda onda de inimigos na fase 1

            }
            else {// Se a onda da fase 1 for 2, cria o chefe da fase 1 (tipo 1) e marca o chefe como ativo
                ChefeCriar(fases, 1);
            }
        }
    }

    //Movimento e ataques de chefes.
    if (fases->chefe.ativo){// Se o chefe estiver ativo, atualiza sua posição, verifica colisões com as bordas da tela e gerencia seus ataques
        Chefe *c = &fases->chefe;// Define um ponteiro para o chefe da fase atual

        c->corpo.x += (float)c->direcao * c->velocidade * dt;// Atualiza a posição horizontal do chefe com base na direção e na velocidade, multiplicando pelo delta de tempo (dt) para garantir movimento suave e consistente

        if (c->corpo.x < 10.0f){
        // Se a posição horizontal do chefe for menor que 10.0f (atingiu a borda esquerda da tela), ajusta sua posição para 10.0f e inverte a direção para a direita
            c->corpo.x = 10.0f;// Ajusta a posição horizontal do chefe para 10.0f, garantindo que ele não ultrapasse a borda esquerda da tela

            c->direcao = 1;// Inverte a direção do chefe para a direita (1), permitindo que ele se mova de volta para dentro da tela
        }
        if (c->corpo.x + c->corpo.width > LARGURA_TELA - 10.0f){
    // Se a posição horizontal do chefe mais sua largura for maior que LARGURA_TELA - 10.0f (atingiu a borda direita da tela), ajusta sua posição para a borda direita e inverte a direção para a esquerda
            c->corpo.x = LARGURA_TELA - 10.0f - c->corpo.width;// Ajusta a posição horizontal do chefe para a borda direita da tela, garantindo que ele não ultrapasse a borda direita

            c->direcao = -1;// Inverte a direção do chefe para a esquerda (-1), permitindo que ele se mova de volta para dentro da tela
        }

        c->intervaloDisparo -= dt;// Reduz o intervalo de disparo do chefe com base no delta de tempo (dt), permitindo que ele dispare projéteis em intervalos regulares

        if (c->intervaloDisparo <= 0.0f){// Se o intervalo de disparo do chefe for menor ou igual a 0.0f, significa que ele está pronto para disparar projéteis
            Rectangle tiro = {c->corpo.x + c->corpo.width / 2.0f - 4.0f, c->corpo.y + c->corpo.height, 8.0f, 16.0f};// Define a posição e o tamanho do projétil que o chefe irá disparar, centralizando-o horizontalmente em relação ao corpo do chefe e posicionando-o logo abaixo do corpo

            ProjeteisAdicionar(projeteis, tiro, 260.0f + c->tipo * 25.0f, PROJETIL_INIMIGO);// Adiciona o projétil disparado pelo chefe à lista de projéteis, com velocidade vertical baseada no tipo do chefe (quanto maior o tipo, maior a velocidade do projétil)

            if (c->tipo >= 2){// Se o tipo do chefe for 2 ou maior (miniboss ou chefe final), dispara projéteis adicionais em posições laterais para aumentar a dificuldade do ataque
                Rectangle tiroEsq = {c->corpo.x + 22.0f, c->corpo.y + c->corpo.height, 7.0f, 14.0f};// Define a posição e o tamanho do projétil disparado pelo lado esquerdo do chefe, posicionando-o próximo à borda esquerda do corpo do chefe

                Rectangle tiroDir = {c->corpo.x + c->corpo.width - 29.0f, c->corpo.y + c->corpo.height, 7.0f, 14.0f};// Define a posição e o tamanho do projétil disparado pelo lado direito do chefe, posicionando-o próximo à borda direita do corpo do chefe

                ProjeteisAdicionar(projeteis, tiroEsq, 250.0f, PROJETIL_INIMIGO);// Adiciona o projétil disparado pelo lado esquerdo do chefe à lista de projéteis, com velocidade vertical fixa de 250.0f

                ProjeteisAdicionar(projeteis, tiroDir, 250.0f, PROJETIL_INIMIGO);// Adiciona o projétil disparado pelo lado direito do chefe à lista de projéteis, com velocidade vertical fixa de 250.0f
            }
            c->intervaloDisparo = c->tipo == 3 ? 0.52f : 0.9f;// Reinicia o intervalo de disparo do chefe com base no tipo do chefe (quanto maior o tipo, menor o intervalo de disparo)
        }
    } 
    else if (fases->chefe.tipo == 1 && fases->chefe.vida == 0){// Se o tipo do chefe for 1 (chefe da fase 1) e sua vida for 0, inicia a fase 2 com o miniboss (tipo 2)
        FaseComecar(fases, inimigos, projeteis, 2);
    } 
    else if (fases->chefe.tipo == 2 && fases->chefe.vida == 0){// Se o tipo do chefe for 2 (miniboss) e sua vida for 0, inicia a fase 3 com o chefe final (tipo 3)
        FaseComecar(fases, inimigos, projeteis, 3);
    } 
    else if (fases->chefe.tipo == 3 && fases->chefe.vida == 0){// Se o tipo do chefe for 3 (chefe final) e sua vida for 0, marca o estado do jogo como vitória, indicando que o jogador venceu o jogo
        *estado = JOGO_VITORIA;
    }
}

void FasesDesenhar(const Fases *fases){
    if (fases == NULL || !fases->chefe.ativo){// Verifica se o ponteiro de fases é nulo ou se o chefe não está ativo antes de desenhar o chefe da fase
         return;
    }
    const Chefe *c = &fases->chefe;// Define um ponteiro constante para o chefe da fase atual, permitindo acessar suas propriedades sem modificá-las

    Color cor = c->tipo == 1 ? MAROON : (c->tipo == 2 ? PURPLE : DARKPURPLE);// Define a cor do chefe com base no tipo do chefe (tipo 1 = marrom, tipo 2 = roxo, tipo 3 = roxo escuro)

    DrawRectangleRounded(c->corpo, 0.16f, 8, cor);// Desenha o corpo do chefe como um retângulo arredondado com a cor definida anteriormente

    DrawRectangle((int)c->corpo.x + 22, (int)c->corpo.y + 18, 35, 12, WHITE);// Desenha a parte frontal do chefe (como uma "boca" ou "visor") como um retângulo branco na posição relativa ao corpo do chefe

    DrawRectangle((int)(c->corpo.x + c->corpo.width - 57), (int)c->corpo.y + 18, 35, 12, WHITE);// Desenha a parte frontal do chefe como um retângulo branco na posição relativa ao corpo do chefe, mas no lado oposto

    const char *nome = c->tipo == 1 ? "Escorpião" : (c->tipo == 2 ? "Hulk" : "CHEFE FINAL");// Define o nome do chefe com base no tipo do chefe (tipo 1 = "Escorpião", tipo 2 = "Hulk", tipo 3 = "CHEFE FINAL")

    DrawText(nome, LARGURA_TELA / 2 - 60, 18, 20, RAYWHITE);// Desenha o nome do chefe na tela, centralizado horizontalmente e posicionado verticalmente a 18 pixels do topo da tela, com tamanho de fonte 20 e cor branca

    const float largura = 300.0f;// Define a largura da barra de vida do chefe como 300 pixels

    float fracao = c->vidaMaxima > 0 ? (float)c->vida / (float)c->vidaMaxima : 0.0f;// Calcula a fração da vida atual do chefe em relação à vida máxima, garantindo que não haja divisão por zero (se a vida máxima for maior que 0, calcula a fração; caso contrário, define como 0.0f)

    if (fracao < 0.0f){// Garante que a fração da vida não seja negativa, caso a vida atual do chefe seja menor que 0
         fracao = 0.0f;
    }
    DrawRectangle((int)(LARGURA_TELA / 2 - largura / 2), 43, (int)largura, 12, DARKGRAY);// Desenha a barra de fundo da vida do chefe como um retângulo cinza escuro, centralizado horizontalmente na tela e posicionado verticalmente a 43 pixels do topo da tela, com largura definida anteriormente e altura de 12 pixels

    DrawRectangle((int)(LARGURA_TELA / 2 - largura / 2), 43, (int)(largura * fracao), 12, RED);// Desenha a barra de vida atual do chefe como um retângulo vermelho, centralizado horizontalmente na tela e posicionado verticalmente a 43 pixels do topo da tela, com largura proporcional à fração da vida atual em relação à vida máxima e altura de 12 pixels

    DrawRectangleLines((int)(LARGURA_TELA / 2 - largura / 2), 43, (int)largura, 12, RAYWHITE);// Desenha a borda da barra de vida do chefe como um retângulo branco, centralizado horizontalmente na tela e posicionado verticalmente a 43 pixels do topo da tela, com largura definida anteriormente e altura de 12 pixels
}
