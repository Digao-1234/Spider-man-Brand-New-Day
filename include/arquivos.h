#ifndef ARQUIVOS_H

#define ARQUIVOS_H

int ArquivosLerRecorde(void);// Retorna o recorde salvo no arquivo, ou 0 se não houver recorde salvo

void ArquivosSalvarRecordeSeMaior(int pontuacao);// Salva a pontuação no arquivo se ela for maior que o recorde atual

#endif