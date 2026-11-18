#ifndef CONFIG_H

#define CONFIG_H


#define LARGURA_TELA 960
#define ALTURA_TELA 640
#define LINHA_BASE 550.0f


// Corredor jogavel de cada fase: a parte do cenario entre as paredes laterais, em pixels da tela (960 de largura).
// Inimigos e chefes nao passam desses limites. Se trocar a imagem de fundo de uma fase, meca estes valores de novo.
static inline float CenarioLimiteEsquerdo(int fase){
    switch (fase){
        case 1:  return 165.0f;
        case 3:  return 173.0f;
        default: return 10.0f;// Fase 2: chao aberto
    }
}

static inline float CenarioLimiteDireito(int fase){
    switch (fase){
        case 1:  return 760.0f;
        case 3:  return 780.0f;
        default: return 800.0f;// Fase 2: os consoles do cenario comecam perto de 805
    }
}

#endif