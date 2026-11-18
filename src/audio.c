#include "audio.h"

#include "raylib.h"

#include <stdbool.h>

#include <stddef.h>

#define COPIAS_POR_SOM 3 // Copias de cada efeito: permite dois tiros seguidos se sobreporem sem cortar o anterior

#define ARQUIVO_MUSICA_MENU "assets/sons/musica_menu.ogg"

#define VOLUME_MUSICA_MENU 0.45f // 0.0 (mudo) a 1.0 (maximo)

typedef struct{
    const char *arquivo; 
    // Caminho do arquivo (relativo a pasta raiz do projeto, como assets/sprites). NULL = ainda sem som

    float volume; 
    // 0.0 a 1.0. Os arquivos ja foram nivelados, entao aqui so se ajusta a "importancia" de cada som

    Sound copias[COPIAS_POR_SOM];
    // Copias do mesmo efeito: para nao cortar o anterior se dois tiros se sobreporem

    int carregadas; // Quantas copias carregaram de verdade

    int proxima; // Qual copia toca no proximo disparo (revezamento)
} Efeito;


static Efeito efeitos[SOM_TOTAL] = {
    [SOM_TIRO_JOGADOR] = {"assets/sons/tiro_jogador.wav", 0.45f}, // O jogador atira o tempo todo, entao fica mais baixo
    [SOM_TIRO_NINJA]   = {"assets/sons/tiro_ninja.wav",   0.60f},
    [SOM_TIRO_GUARDA]  = {"assets/sons/tiro_guarda.wav",  0.60f},
    [SOM_TIRO_BOSS1]   = {"assets/sons/tiro_boss1.wav",   0.80f},
    [SOM_TIRO_BOSS2]   = {"assets/sons/tiro_boss2.wav",   0.80f} 
};

static bool audioPronto = false; 
// false se nao houver dispositivo de audio: o jogo continua, so que mudo

static Music musicaMenu;
// A musica do menu e lida aos poucos do arquivo (streaming)

static bool musicaCarregada = false;
// true se a musica do menu foi carregada com sucesso

static bool musicaTocando = false;
// true se a musica do menu esta tocando

void AudioCarregar(void){
    InitAudioDevice();// Liga o dispositivo de audio

    audioPronto = IsAudioDeviceReady();
    // true se o dispositivo de audio foi ligado com sucesso

    if (!audioPronto){
        TraceLog(LOG_WARNING, "AVISO: dispositivo de audio indisponivel");
        // Nao da para tocar som, mas o jogo continua

        return;
    }

    for (int i = 0; i < SOM_TOTAL; i++){// Para cada efeito sonoro
        Efeito *e = &efeitos[i];// Ponteiro para o efeito atual

        if (e->arquivo == NULL){
            continue;// Efeito ainda sem arquivo
        }

        for (int c = 0; c < COPIAS_POR_SOM; c++){// Para cada copia do efeito
            Sound s = LoadSound(e->arquivo);// Carrega o som do arquivo

            if (s.frameCount == 0){// Falhou ao carregar o som
                TraceLog(LOG_WARNING, "AVISO: nao foi possivel carregar %s", e->arquivo);
                // Nao da para tocar som, mas o jogo continua

                break;
            }

            SetSoundVolume(s, e->volume);// Ajusta o volume do som

            e->copias[e->carregadas] = s;// Salva a copia carregada

            e->carregadas++;// Conta quantas copias carregaram de verdade
        }
    }

    musicaMenu = LoadMusicStream(ARQUIVO_MUSICA_MENU);// A musica e lida aos poucos do arquivo (streaming)

    musicaCarregada = musicaMenu.frameCount > 0;// true se a musica do menu foi carregada com sucesso

    if (musicaCarregada){// true se a musica do menu foi carregada com sucesso
        musicaMenu.looping = true;// Volta ao inicio quando termina

        SetMusicVolume(musicaMenu, VOLUME_MUSICA_MENU);// Ajusta o volume da musica do menu
    }
    else{// Falhou ao carregar a musica do menu
        TraceLog(LOG_WARNING, "AVISO: nao foi possivel carregar %s", ARQUIVO_MUSICA_MENU);
    }
}

void AudioDescarregar(void){// Libera os sons e desliga o audio. Chamar antes de CloseWindow()
    if (!audioPronto){// Nao da para tocar som, mas o jogo continua
        return;
    }

    if (musicaCarregada){// true se a musica do menu foi carregada com sucesso
        StopMusicStream(musicaMenu);// Para e volta ao inicio da musica

        UnloadMusicStream(musicaMenu);// Libera a musica do menu

        musicaCarregada = false;// true se a musica do menu foi carregada com sucesso

        musicaTocando = false;// true se a musica do menu esta tocando
    }

    for (int i = 0; i < SOM_TOTAL; i++){// Para cada efeito sonoro
        for (int c = 0; c < efeitos[i].carregadas; c++){// Para cada copia do efeito
            UnloadSound(efeitos[i].copias[c]);// Libera a copia do efeito
        }

        efeitos[i].carregadas = 0;// Conta quantas copias carregaram de verdade
    }

    CloseAudioDevice();// Desliga o dispositivo de audio

    audioPronto = false;// false se nao houver dispositivo de audio: o jogo continua, so que mudo
}

void AudioAtualizar(void){// Chamar UMA vez por quadro: mantem a musica tocando
    if (audioPronto && musicaCarregada && musicaTocando){// true se a musica do menu esta tocando
        UpdateMusicStream(musicaMenu);// Mantem a musica tocando
        
    }
}

void AudioTocar(SomId id){// Toca um efeito sonoro (nao faz nada se o som nao carregou)
    if (!audioPronto || (int)id < 0 || (int)id >= SOM_TOTAL){// Nao da para tocar som, mas o jogo continua
        return;
    }

    Efeito *e = &efeitos[id];// Ponteiro para o efeito atual

    if (e->carregadas == 0){
        return;// Sem arquivo ou falhou ao carregar
    }

    PlaySound(e->copias[e->proxima]);// Toca a copia do efeito

    e->proxima = (e->proxima + 1) % e->carregadas;// Revezamento entre as copias do efeito
}

void AudioMusicaMenuIniciar(void){// Comeca a musica do menu do inicio (em loop)
    if (!audioPronto || !musicaCarregada || musicaTocando){
        return;// Nao da para tocar som, mas o jogo continua
    }

    PlayMusicStream(musicaMenu);// Comeca a musica do menu do inicio (em loop)

    musicaTocando = true;// true se a musica do menu esta tocando
}

void AudioMusicaMenuParar(void){
    if (!audioPronto || !musicaCarregada || !musicaTocando){
        return;// Nao da para tocar som, mas o jogo continua
    }

    StopMusicStream(musicaMenu);// Para e volta ao inicio da musica

    musicaTocando = false;// true se a musica do menu esta tocando
}