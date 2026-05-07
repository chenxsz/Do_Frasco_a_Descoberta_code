#include "jogador.h"

void InicializarJogador(Jogador *j) {
    j->posicao = (Vector2){ 480, 320 };
    j->pontosFase = 0;
    j->pontosTotais = 0;
    j->idPersonagemAtual = 0; 
    j->estadoAnimacao = 0;    
    j->frameAtual = 0;
    j->tempoFrame = 0;
    j->timerAgachado = 0;

    j->catalogo[0] = (Cientista){"Marie Curie", true};
    j->catalogo[1] = (Cientista){"Rosalind Franklin", false};
    j->catalogo[2] = (Cientista){"Ada Lovelace", false};
    j->catalogo[3] = (Cientista){"Katherine Johnson", false};
    j->catalogo[4] = (Cientista){"Bertha Lutz", false};
}

void AtualizarMovimento(Jogador *j, float delta) {
    float velocidade = 250.0f;
    if (j->estadoAnimacao != 2) {
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) j->posicao.x += velocidade * delta;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  j->posicao.x -= velocidade * delta;
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    j->posicao.y -= velocidade * delta;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  j->posicao.y += velocidade * delta;
    }
}

void VerificarDesbloqueio(Jogador *j) {
    if (j->pontosTotais >= 25) j->catalogo[1].desbloqueado = true;
    if (j->pontosTotais >= 50) j->catalogo[2].desbloqueado = true;
    if (j->pontosTotais >= 75) j->catalogo[3].desbloqueado = true;
    if (j->pontosTotais >= 100) j->catalogo[4].desbloqueado = true;
}