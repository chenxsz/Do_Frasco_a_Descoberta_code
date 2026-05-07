#ifndef JOGADOR_H
#define JOGADOR_H

#include "include/raylib/raylib.h"

typedef struct {
    char nome[50];
    bool desbloqueado;
} Cientista;

typedef enum {
    TELA_MENU,
    TELA_JOGANDO,
    TELA_INVENTARIO,
    TELA_CATALOGO,
    TELA_CREDITOS,
    TELA_QUIZ
} EstadoJogo;

typedef struct {
    Vector2 posicao;
    int pontosFase;
    int pontosTotais;
    int idPersonagemAtual;
    int estadoAnimacao;    
    int frameAtual;
    float tempoFrame;
    float timerAgachado;   

    Cientista catalogo[5]; 
} Jogador;

void InicializarJogador(Jogador *j);
void AtualizarMovimento(Jogador *j, float delta);
void VerificarDesbloqueio(Jogador *j);

#endif