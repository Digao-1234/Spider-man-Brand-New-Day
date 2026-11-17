#include "arquivos.h"

#include "raylib.h"

#include <stdio.h>

#define CAMINHO_RECORDE "saves/recorde.txt"

int ArquivosLerRecorde(void){
    FILE *arquivo = fopen(CAMINHO_RECORDE, "r");// Abre o arquivo de recorde para leitura

    int recorde = 0;// Inicializa o recorde como 0

    if (arquivo == NULL){// Verifica se o arquivo foi aberto com sucesso
         return 0;// Retorna 0 se não houver arquivo de recorde
    }
    if (fscanf(arquivo, "%d", &recorde) != 1 || recorde < 0){
         recorde = 0;// Se o arquivo estiver vazio ou contiver um valor inválido, define o recorde como 0
    }
    fclose(arquivo);// Fecha o arquivo de recorde

    return recorde;// Retorna o recorde lido do arquivo
}

void ArquivosSalvarRecordeSeMaior(int pontuacao){
    if (pontuacao <= ArquivosLerRecorde()){// Verifica se a pontuação atual é maior que o recorde salvo
         return;// Retorna se a pontuação não for maior que o recorde atual
    }
    FILE *arquivo = fopen(CAMINHO_RECORDE, "w");// Abre o arquivo de recorde para escrita, sobrescrevendo o conteúdo existente

    if (arquivo == NULL){// Verifica se o arquivo foi aberto com sucesso
        TraceLog(LOG_WARNING, "Nao foi possivel escrever em %s", CAMINHO_RECORDE);// Registra um aviso se não for possível abrir o arquivo para escrita

        return;
    }
    fprintf(arquivo, "%d\n", pontuacao);// Escreve a nova pontuação no arquivo de recorde

    fclose(arquivo);// Fecha o arquivo de recorde
}