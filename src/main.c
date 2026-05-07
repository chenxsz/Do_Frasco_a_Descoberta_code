#include <stdio.h>
#include <stdlib.h>
#include "include/raylib/raylib.h"
#include "jogador.h"

#define NUM_PERSONAGENS 5
#define NUM_FRAMES_ANDAR 1
#define NUM_FRAMES_PARADO 1
#define NUM_TIPOS_FRASCOS 3


typedef struct {
    Texture2D parado[NUM_FRAMES_PARADO];
    Texture2D andar[NUM_FRAMES_ANDAR];
    Texture2D pegar; 
} AnimacaoPersonagem;

typedef struct {
    Vector2 posicao;
    int dificuldade; 
    bool ativo;
} Frasco;

typedef struct {
    Texture2D menu;
    Texture2D fundoJogo;
    AnimacaoPersonagem personagens[NUM_PERSONAGENS];
    Texture2D frascos[NUM_TIPOS_FRASCOS];
} Recursos;


EstadoJogo estadoAtual = TELA_MENU;
Jogador player;
Frasco frascos[3];
Recursos assets;
int dificuldadeQuiz = 0;


void CarregarRecursos(void);
void DescarregarRecursos(void);
void InicializarFrascos(void);
void AtualizarAnimacaoJogador(Jogador *p, float delta);
void update(float delta);
void draw(void);


int main(void) {
    InitWindow(960, 640, "Do Frasco a Descoberta - IFSP SJBV");
    SetTargetFPS(60);

    CarregarRecursos();
    InicializarJogador(&player);
    InicializarFrascos();

    while (!WindowShouldClose()) {
        update(GetFrameTime());
        draw();
    }

    DescarregarRecursos();
    CloseWindow();
    return 0;
}

void CarregarRecursos(void) {
    assets.menu = LoadTexture("imagens/fundo/fundo_menu.png");
    assets.fundoJogo = LoadTexture("imagens/fundo/fundo_fase.png");

    for (int p = 0; p < NUM_PERSONAGENS; p++) {
        for (int i = 0; i < NUM_FRAMES_PARADO; i++) {
            assets.personagens[p].parado[i] = LoadTexture(TextFormat("imagens/personagens/personagem_%d/parado_%d.png", p, i));
        }
        for (int i = 0; i < NUM_FRAMES_ANDAR; i++) {
            assets.personagens[p].andar[i] = LoadTexture(TextFormat("imagens/personagens/personagem_%d/andar_%d.png", p, i));
        }
        assets.personagens[p].pegar = LoadTexture(TextFormat("imagens/personagens/personagem_%d/pegar.png", p));
    }

    for (int i = 0; i < NUM_TIPOS_FRASCOS; i++) {
        assets.frascos[i] = LoadTexture(TextFormat("imagens/frascos/frasco%d.png", i));
    }
}

void DescarregarRecursos(void) {
    UnloadTexture(assets.menu);
    UnloadTexture(assets.fundoJogo);
    for (int p = 0; p < NUM_PERSONAGENS; p++) {
        for (int i = 0; i < NUM_FRAMES_PARADO; i++) { UnloadTexture(assets.personagens[p].parado[i]); }
        for (int i = 0; i < NUM_FRAMES_ANDAR; i++) { UnloadTexture(assets.personagens[p].andar[i]); }
        UnloadTexture(assets.personagens[p].pegar);
    }
    for (int i = 0; i < NUM_TIPOS_FRASCOS; i++) { 
        UnloadTexture(assets.frascos[i]); 
    }
}

void InicializarFrascos(void) {
    for (int i = 0; i < 3; i++) {
        frascos[i].posicao = (Vector2){ (float)GetRandomValue(100, 800), (float)GetRandomValue(150, 500) };
        frascos[i].dificuldade = i; 
        frascos[i].ativo = true;
    }
}

void AtualizarAnimacaoJogador(Jogador *p, float delta) {
    if (p->estadoAnimacao == 2) return; 

    float velAnim = 0.15f; 
    p->tempoFrame += delta;

    if (p->tempoFrame >= velAnim) {
        p->tempoFrame = 0.0f;
        if (p->estadoAnimacao == 0) {
            p->frameAtual = (p->frameAtual + 1) % NUM_FRAMES_PARADO;
        } 
        else if (p->estadoAnimacao == 1) {
            p->frameAtual = (p->frameAtual + 1) % NUM_FRAMES_ANDAR;
        }
    }
}

void update(float delta) {
    Rectangle btnJogar = { 380, 250, 200, 40 };
    Rectangle btnInventario = { 380, 310, 200, 40 };
    Rectangle btnCatalogo = { 380, 370, 200, 40 };
    Rectangle btnCreditos = { 380, 430, 200, 40 };

    switch (estadoAtual) {
        case TELA_MENU:
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mPos = GetMousePosition();
                if (CheckCollisionPointRec(mPos, btnJogar)) { 
                    estadoAtual = TELA_JOGANDO; 
                } else if (CheckCollisionPointRec(mPos, btnInventario)) { 
                    estadoAtual = TELA_INVENTARIO; 
                } else if (CheckCollisionPointRec(mPos, btnCatalogo)) {
                     estadoAtual = TELA_CATALOGO; 
                } else if (CheckCollisionPointRec(mPos, btnCreditos)) { 
                    estadoAtual = TELA_CREDITOS; 
                }
            }
            break;

        case TELA_JOGANDO:
            if (player.estadoAnimacao == 2) {
                player.timerAgachado += delta;
                if (player.timerAgachado > 0.6f) { 
                    player.estadoAnimacao = 0;
                    player.timerAgachado = 0;
                    player.frameAtual = 0;
                }
            } 
            else {
                bool mov = (IsKeyDown(KEY_D) || IsKeyDown(KEY_A) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S));
                
                if (mov) {
                    if (player.estadoAnimacao != 1) {
                        player.estadoAnimacao = 1;
                        player.frameAtual = 0;
                    }
                } else {
                    if (player.estadoAnimacao != 0) {
                        player.estadoAnimacao = 0;
                        player.frameAtual = 0;
                    }
                }

                if (IsKeyPressed(KEY_F)) {
                    for (int i = 0; i < 3; i++) {
                        if (frascos[i].ativo && CheckCollisionCircles(player.posicao, 20, frascos[i].posicao, 25)) {
                            player.estadoAnimacao = 2;
                            player.timerAgachado = 0;
                            dificuldadeQuiz = frascos[i].dificuldade;
                            frascos[i].ativo = false;
                            estadoAtual = TELA_QUIZ;
                            break;
                        }
                    }
                }
            }
            AtualizarMovimento(&player, delta);
            AtualizarAnimacaoJogador(&player, delta);
            break;

        case TELA_QUIZ:
            if (IsKeyPressed(KEY_ONE)) {
                player.pontosTotais += 25;
                VerificarDesbloqueio(&player);
                estadoAtual = TELA_JOGANDO;
                bool algumAtivo = false;
                for (int i = 0; i < 3; i++) { if (frascos[i].ativo) algumAtivo = true; }
                if (!algumAtivo) InicializarFrascos();
            }
            if (IsKeyPressed(KEY_TWO)) { estadoAtual = TELA_JOGANDO; }
            break;

        default:
            if (IsKeyPressed(KEY_M)) { estadoAtual = TELA_MENU; }
            break;
    }
}

void draw(void) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (estadoAtual == TELA_MENU) {
        DrawTexture(assets.menu, 0, 0, WHITE);
        DrawText("DO FRASCO A DESCOBERTA", 220, 100, 40, DARKGREEN);
        
        DrawRectangle(380, 250, 200, 40, DARKGREEN); DrawText("JOGAR", 450, 260, 20, WHITE);
        DrawRectangle(380, 310, 200, 40, DARKGREEN); DrawText("INVENTARIO", 425, 320, 20, WHITE);
        DrawRectangle(380, 370, 200, 40, DARKGREEN); DrawText("CATALOGO", 435, 380, 20, WHITE);
        DrawRectangle(380, 430, 200, 40, DARKBLUE);  DrawText("CREDITOS", 440, 440, 20, WHITE);
    } 
    else if (estadoAtual == TELA_JOGANDO) {
        DrawTexture(assets.fundoJogo, 0, 0, WHITE);
        for (int i = 0; i < 3; i++) {
            if (frascos[i].ativo) {
                DrawTextureV(assets.frascos[frascos[i].dificuldade], frascos[i].posicao, WHITE);
            }
        }

        Texture2D tex;
        int id = player.idPersonagemAtual;
        
        if (player.estadoAnimacao == 2) {
            tex = assets.personagens[id].pegar;
        } 
        else if (player.estadoAnimacao == 1) {
            tex = assets.personagens[id].andar[player.frameAtual];
        } 
        else {
            tex = assets.personagens[id].parado[player.frameAtual];
        }

        Vector2 posFinal = { player.posicao.x - tex.width/2.0f, player.posicao.y - tex.height/2.0f };
        
        if (tex.id != 0) {
            DrawTextureV(tex, posFinal, WHITE);
        } else {
            DrawCircleV(player.posicao, 20, MAROON);
        }

        DrawText(TextFormat("Pontos: %d", player.pontosTotais), 20, 20, 20, BLACK);
    }
    else if (estadoAtual == TELA_CREDITOS) {
        DrawText("IFSP - Campus Sao Joao da Boa Vista", 280, 140, 20, BLACK);
        DrawText("INTEGRANTES:", 280, 195, 20, MAROON);
        DrawText("FERNANDA CHEN\nGLEYCE KELLY\nMILENA CRISTINA\nSOFIA DE BARROS\nYONA MAIA", 280, 230, 20, BLACK);
        DrawText("Pressione M para voltar", 330, 550, 20, GRAY);
    }
    
    EndDrawing();
}