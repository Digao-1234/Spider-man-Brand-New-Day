#ifndef AUDIO_H

#define AUDIO_H

typedef enum{ // Identificador de cada efeito sonoro (SOM_TOTAL deve ficar por ultimo)
    SOM_TIRO_JOGADOR = 0,
    SOM_TIRO_NINJA,
    SOM_TIRO_GUARDA,
    SOM_TIRO_BOSS1,
    SOM_TIRO_BOSS2,
    SOM_TIRO_BOSS3,
    SOM_TOTAL
} SomId;

void AudioCarregar(void); // Liga o audio e carrega os sons. Chamar UMA vez, depois de InitWindow()

void AudioDescarregar(void); // Libera os sons e desliga o audio. Chamar antes de CloseWindow()

void AudioAtualizar(void); // Chamar UMA vez por quadro: mantem a musica tocando

void AudioTocar(SomId id); // Toca um efeito sonoro (nao faz nada se o som nao carregou)

void AudioMusicaMenuIniciar(void); // Comeca a musica do menu do inicio (em loop)

void AudioMusicaMenuParar(void); // Para a musica do menu

#endif