#include <stdio.h>
#include <stdlib.h>
#include "include/raylib/raylib.h"
#include "jogador.h"

#define NUM_PERSONAGENS 5
#define NUM_FRAMES_ANDAR 1
#define NUM_FRAMES_PARADO 1
#define NUM_TIPOS_FRASCOS 3
#define FRASCO_ALTURA_ALVO 90.0f
#define FRASCO_RAIO_COLISAO 40.0f
#define JOGADOR_RAIO_COLISAO 20.0f
#define PONTOS_PARA_VENCER 300
#define TEMPO_INICIAL 120.0f

typedef struct {
    Texture2D parado[NUM_FRAMES_PARADO];
    Texture2D andarDireita[NUM_FRAMES_ANDAR];
    Texture2D andarEsquerda[NUM_FRAMES_ANDAR];
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

#define NUM_PERGUNTAS_POR_DIFICULDADE 2

typedef struct {
    const char *pergunta;
    const char *alternativaA;
    const char *alternativaB;
    int respostaCorreta;      
    const char *curiosidade;  
} PerguntaQuiz;

PerguntaQuiz bancoPerguntas[NUM_TIPOS_FRASCOS][NUM_PERGUNTAS_POR_DIFICULDADE] = {
    {
        {
            "Marie Curie foi uma cientista muito importante.\nEla estudava coisas bem pequenininhas que soltam\nenergia. Isso se chama:",
            "Sanduiche",
            "Radioatividade",
            2,
            "Marie Curie descobriu o polonio e o radio, e foi\na unica pessoa da historia a ganhar DOIS Premios\nNobel, em areas diferentes!"
        },
        {
            "Katherine Johnson era muito boa em contas de\nmatematica. Ela ajudou a mandar foguetes para:",
            "O espaco",
            "O fundo do mar",
            1,
            "Os calculos de Katherine Johnson ajudaram\nastronautas da NASA a chegarem ao espaco\ncom seguranca!"
        }
    },
    {
        {
            "A cientista brasileira Bertha Lutz lutou muito\npara que as mulheres pudessem:",
            "Jogar futebol",
            "Votar nas eleicoes",
            2,
            "Bertha Lutz ajudou a conquistar o direito das\nmulheres brasileiras de votar, isso aconteceu\nem 1932!"
        },
        {
            "Rosalind Franklin tirou fotos especiais que\najudaram a descobrir o formato de uma coisinha\nque existe dentro do nosso corpo, o:",
            "DNA",
            "Cabelo",
            1,
            "O DNA guarda as informacoes que fazem voce\nser do seu jeitinho! As fotos de Rosalind\najudaram a descobrir o formato dele."
        }
    },
    {
        {
            "Tu Youyou estudou plantas e descobriu um\nremedio que ajudou muitas pessoas doentes de:",
            "Dor de barriga",
            "Malaria",
            2,
            "O remedio de Tu Youyou se chama artemisinina\ne salvou milhoes de vidas! Ela ganhou o Premio\nNobel por essa descoberta."
        },
        {
            "Mary Anning adorava procurar coisas na praia.\nEla encontrou ossos gigantes de animais que\nviveram ha muito, muito tempo. Isso se chama:",
            "Fosseis",
            "Brinquedos antigos",
            1,
            "Mary Anning comecou a procurar fosseis ainda\ncrianca, e suas descobertas ajudaram a criar a\nciencia da Paleontologia!"
        }
    }
};

int perguntaSelecionada = 0;   
bool quizRespondido = false;   
bool respostaCorreta = false;
int escolhaFeita = 0;          
float timerFeedback = 0.0f;
float tempoRestante = TEMPO_INICIAL;

EstadoJogo estadoAtual = TELA_MENU;
Jogador player;
Frasco frascos[3];
Recursos assets;
int dificuldadeQuiz = 0;

bool olhandoEsquerda = false;

void CarregarRecursos(void);
void DescarregarRecursos(void);
void InicializarFrascos(void);
void ReiniciarJogo(void);
void AtualizarAnimacaoJogador(Jogador *p, float delta);
void DesenharFrasco(Texture2D tex, Vector2 centro);
void update(float delta);
void draw(void);

void DrawTextCentered(const char *text, int posY, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    int posX = (GetScreenWidth() - textWidth) / 2;
    DrawText(text, posX, posY, fontSize, color);
}

int main(void) {
    InitWindow(960, 640, "Do Frasco à Descoberta - IFSP SJBV");
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
            assets.personagens[p].andarDireita[i] = LoadTexture(TextFormat("imagens/personagens/personagem_%d/andar_%d_direita.png", p, i));
            assets.personagens[p].andarEsquerda[i] = LoadTexture(TextFormat("imagens/personagens/personagem_%d/andar_%d_esquerda.png", p, i));
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
        for (int i = 0; i < NUM_FRAMES_ANDAR; i++) { 
            UnloadTexture(assets.personagens[p].andarDireita[i]); 
            UnloadTexture(assets.personagens[p].andarEsquerda[i]); 
        }
        UnloadTexture(assets.personagens[p].pegar);
    }
    for (int i = 0; i < NUM_TIPOS_FRASCOS; i++) { 
        UnloadTexture(assets.frascos[i]); 
    }
}

void InicializarFrascos(void) {
    for (int i = 0; i < 3; i++) {
        frascos[i].posicao = (Vector2){ (float)GetRandomValue(120, GetScreenWidth() - 120), (float)GetRandomValue(170, GetScreenHeight() - 120) };
        frascos[i].dificuldade = i; 
        frascos[i].ativo = true;
    }
}

void ReiniciarJogo(void) {
    player.pontosTotais = 0;
    tempoRestante = TEMPO_INICIAL;
    InicializarJogador(&player);
    InicializarFrascos();
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

void DesenharFrasco(Texture2D tex, Vector2 centro) {
    float escala = FRASCO_ALTURA_ALVO / (float)tex.height;
    float largura = tex.width * escala;
    float altura = tex.height * escala;

    Rectangle origem  = { 0, 0, (float)tex.width, (float)tex.height };
    Rectangle destino = { centro.x, centro.y, largura, altura };
    Vector2 pivo      = { largura / 2.0f, altura / 2.0f }; 

    DrawTexturePro(tex, origem, destino, pivo, 0.0f, WHITE);
}

void update(float delta) {
    int btnLargura = 220;
    int btnAltura = 45;
    int btnX = (GetScreenWidth() - btnLargura) / 2;
    int btnStartY = GetScreenHeight() / 2 - 60;

    Rectangle btnJogar      = { btnX, btnStartY, btnLargura, btnAltura };
    Rectangle btnInventario = { btnX, btnStartY + 60, btnLargura, btnAltura };
    Rectangle btnCatalogo   = { btnX, btnStartY + 120, btnLargura, btnAltura };
    Rectangle btnCreditos   = { btnX, btnStartY + 180, btnLargura, btnAltura };

    switch (estadoAtual) {
        case TELA_MENU:
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mPos = GetMousePosition();
                if (CheckCollisionPointRec(mPos, btnJogar)) { 
                    ReiniciarJogo();
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
            tempoRestante -= delta;
            if (tempoRestante <= 0.0f) {
                tempoRestante = 0.0f;
                estadoAtual = TELA_GAME_OVER;
                break;
            }

            if (player.pontosTotais >= PONTOS_PARA_VENCER) {
                estadoAtual = TELA_VITORIA;
                break;
            }

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
                
                if (IsKeyDown(KEY_A)) {
                    olhandoEsquerda = true;
                } else if (IsKeyDown(KEY_D)) {
                    olhandoEsquerda = false;
                }

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

                if(IsKeyPressed(KEY_M)) {
                    estadoAtual = TELA_MENU;
                    break;
                }

                if (IsKeyPressed(KEY_F)) {
                    for (int i = 0; i < 3; i++) {
                        if (frascos[i].ativo && CheckCollisionCircles(player.posicao, JOGADOR_RAIO_COLISAO, frascos[i].posicao, FRASCO_RAIO_COLISAO)) {
                            player.estadoAnimacao = 2;
                            player.timerAgachado = 0;
                            dificuldadeQuiz = frascos[i].dificuldade;
                            perguntaSelecionada = GetRandomValue(0, NUM_PERGUNTAS_POR_DIFICULDADE - 1);
                            quizRespondido = false;
                            timerFeedback = 0.0f;
                            frascos[i].ativo = false;
                            estadoAtual = TELA_QUIZ;
                            break;
                        }
                    }
                }
            }

            AtualizarMovimento(&player, delta);

            if (player.posicao.x - JOGADOR_RAIO_COLISAO < 0) {
                player.posicao.x = JOGADOR_RAIO_COLISAO;
            }
            if (player.posicao.x + JOGADOR_RAIO_COLISAO > GetScreenWidth()) {
                player.posicao.x = GetScreenWidth() - JOGADOR_RAIO_COLISAO;
            }
            if (player.posicao.y - JOGADOR_RAIO_COLISAO < 0) {
                player.posicao.y = JOGADOR_RAIO_COLISAO;
            }
            if (player.posicao.y + JOGADOR_RAIO_COLISAO > GetScreenHeight()) {
                player.posicao.y = GetScreenHeight() - JOGADOR_RAIO_COLISAO;
            }

            AtualizarAnimacaoJogador(&player, delta);
            break;

        case TELA_QUIZ: {
            PerguntaQuiz *perguntaAtual = &bancoPerguntas[dificuldadeQuiz][perguntaSelecionada];

            if(IsKeyPressed(KEY_M)) {
                estadoAtual = TELA_MENU;
                break;
            }

            if (!quizRespondido) {
                if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_TWO)) {
                    int escolha = IsKeyPressed(KEY_ONE) ? 1 : 2;
                    escolhaFeita = escolha;
                    respostaCorreta = (escolha == perguntaAtual->respostaCorreta);
                    if (respostaCorreta) {
                        player.pontosTotais += 25;
                        VerificarDesbloqueio(&player);
                    }
                    quizRespondido = true;
                    timerFeedback = 0.0f;
                }
            } else {
                timerFeedback += delta;
                if (timerFeedback > 3.0f || IsKeyPressed(KEY_SPACE)) {
                    if (player.pontosTotais >= PONTOS_PARA_VENCER) {
                        estadoAtual = TELA_VITORIA;
                    } else {
                        estadoAtual = TELA_JOGANDO;
                        bool algumAtivo = false;
                        for (int i = 0; i < 3; i++) { if (frascos[i].ativo) algumAtivo = true; }
                        if (!algumAtivo) InicializarFrascos();
                    }
                }
            }
            break;
        }

        case TELA_VITORIA:
        case TELA_GAME_OVER:
            if (IsKeyPressed(KEY_R)) {
                ReiniciarJogo();
                estadoAtual = TELA_JOGANDO;
            } else if (IsKeyPressed(KEY_M)) {
                estadoAtual = TELA_MENU;
            }
            break;

        default:
            if (IsKeyPressed(KEY_M)) { estadoAtual = TELA_MENU; }
            break;
    }
}

void draw(void) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    if (estadoAtual == TELA_MENU) {
        Rectangle origemMenu = { 0, 0, (float)assets.menu.width, (float)assets.menu.height };
        Rectangle destinoMenu = { 0, 0, (float)screenW, (float)screenH };
        DrawTexturePro(assets.menu, origemMenu, destinoMenu, (Vector2){0, 0}, 0.0f, WHITE);
        
        DrawTextCentered("DO FRASCO À DESCOBERTA", screenH / 4, 40, GREEN);
        
        int btnLargura = 220;
        int btnAltura = 45;
        int btnX = (screenW - btnLargura) / 2;
        int btnStartY = screenH / 2 - 60;

        DrawRectangle(btnX, btnStartY, btnLargura, btnAltura, DARKGREEN); 
        DrawTextCentered("JOGAR", btnStartY + 12, 20, WHITE);

        DrawRectangle(btnX, btnStartY + 60, btnLargura, btnAltura, DARKGREEN); 
        DrawTextCentered("INVENTÁRIO", btnStartY + 72, 20, WHITE);

        DrawRectangle(btnX, btnStartY + 120, btnLargura, btnAltura, DARKGREEN); 
        DrawTextCentered("CATÁLOGO", btnStartY + 132, 20, WHITE);

        DrawRectangle(btnX, btnStartY + 180, btnLargura, btnAltura, DARKBLUE);  
        DrawTextCentered("CRÉDITOS", btnStartY + 192, 20, WHITE);
    } 
    else if (estadoAtual == TELA_JOGANDO) {
        Rectangle origemFundo = { 0, 0, (float)assets.fundoJogo.width, (float)assets.fundoJogo.height };
        Rectangle destinoFundo = { 0, 0, (float)screenW, (float)screenH };
        DrawTexturePro(assets.fundoJogo, origemFundo, destinoFundo, (Vector2){0, 0}, 0.0f, WHITE);

        for (int i = 0; i < 3; i++) {
            if (frascos[i].ativo) {
                DesenharFrasco(assets.frascos[frascos[i].dificuldade], frascos[i].posicao);
            }
        }

        Texture2D tex;
        int id = player.idPersonagemAtual;
        
        if (player.estadoAnimacao == 2) {
            tex = assets.personagens[id].pegar;
        } 
        else if (player.estadoAnimacao == 1) {
            tex = olhandoEsquerda ? assets.personagens[id].andarEsquerda[player.frameAtual] 
                                  : assets.personagens[id].andarDireita[player.frameAtual];
        } 
        else {
            tex = assets.personagens[id].parado[player.frameAtual];
        }

        Vector2 posFinal = { player.posicao.x - tex.width/2.0f, player.posicao.y - tex.height/2.0f };
        
        if (tex.id != 0) {
            DrawTextureV(tex, posFinal, WHITE);
        } else {
            DrawCircleV(player.posicao, JOGADOR_RAIO_COLISAO, MAROON);
        }

        int minutos = (int)tempoRestante / 60;
        int segundos = (int)tempoRestante % 60;
        
        DrawText(TextFormat("Pontos: %d / %d", player.pontosTotais, PONTOS_PARA_VENCER), 20, 20, 20, WHITE);
        DrawText(TextFormat("Tempo: %02d:%02d", minutos, segundos), 20, 50, 20, (tempoRestante < 20.0f) ? RED : YELLOW);
        DrawText("Clique F para pegar frasco", 20, 80, 18, GRAY);
        DrawText("Clique M para voltar ao menu", 20, 105, 18, GRAY);
    }
    else if (estadoAtual == TELA_QUIZ) {
        Rectangle origemFundo = { 0, 0, (float)assets.fundoJogo.width, (float)assets.fundoJogo.height };
        Rectangle destinoFundo = { 0, 0, (float)screenW, (float)screenH };
        DrawTexturePro(assets.fundoJogo, origemFundo, destinoFundo, (Vector2){0, 0}, 0.0f, WHITE);

        DrawRectangle(0, 0, screenW, screenH, (Color){ 20, 20, 30, 210 });

        PerguntaQuiz *perguntaAtual = &bancoPerguntas[dificuldadeQuiz][perguntaSelecionada];

        DrawTextCentered("HORA DA DESCOBERTA!", 40, 30, GOLD);
        DrawTextCentered(perguntaAtual->pergunta, 110, 22, WHITE);

        int boxWidth = 800;
        int boxX = (screenW - boxWidth) / 2;

        if (!quizRespondido) {
            DrawRectangle(boxX, 280, boxWidth, 60, DARKBLUE);
            DrawText(TextFormat("[1]  %s", perguntaAtual->alternativaA), boxX + 20, 298, 20, WHITE);

            DrawRectangle(boxX, 360, boxWidth, 60, DARKBLUE);
            DrawText(TextFormat("[2]  %s", perguntaAtual->alternativaB), boxX + 20, 378, 20, WHITE);
        } else {
            Color corA = (perguntaAtual->respostaCorreta == 1) ? GREEN : ((escolhaFeita == 1) ? RED : GRAY);
            Color corB = (perguntaAtual->respostaCorreta == 2) ? GREEN : ((escolhaFeita == 2) ? RED : GRAY);

            DrawRectangle(boxX, 280, boxWidth, 50, corA);
            DrawText(TextFormat("[1]  %s", perguntaAtual->alternativaA), boxX + 20, 295, 18, BLACK);

            DrawRectangle(boxX, 340, boxWidth, 50, corB);
            DrawText(TextFormat("[2]  %s", perguntaAtual->alternativaB), boxX + 20, 355, 18, BLACK);

            if (respostaCorreta) {
                DrawTextCentered("Isso mesmo! Você acertou! (+25 pontos)", 410, 22, GREEN);
            } else {
                DrawTextCentered("Quase! Mas olha só o que você descobriu:", 410, 22, ORANGE);
            }
            DrawTextCentered(perguntaAtual->curiosidade, 450, 18, WHITE);
            DrawTextCentered("Continuando... (ou aperte ESPACO / M)", screenH - 50, 16, LIGHTGRAY);
        }
    } else if (estadoAtual == TELA_VITORIA) {
        Rectangle origemFundo = { 0, 0, (float)assets.fundoJogo.width, (float)assets.fundoJogo.height };
        Rectangle destinoFundo = { 0, 0, (float)screenW, (float)screenH };
        DrawTexturePro(assets.fundoJogo, origemFundo, destinoFundo, (Vector2){0, 0}, 0.0f, WHITE);

        DrawRectangle(0, 0, screenW, screenH, (Color){ 10, 30, 10, 220 });

        DrawTextCentered("PARABÉNS! VOCÊ VENCEU!", screenH / 3, 40, GOLD);
        DrawTextCentered(TextFormat("Você alcançou os %d pontos necessários!", PONTOS_PARA_VENCER), screenH / 2 - 20, 22, WHITE);
        DrawTextCentered("Pressione R para jogar novamente", screenH / 2 + 50, 20, LIGHTGRAY);
        DrawTextCentered("Pressione M para voltar ao menu", screenH / 2 + 90, 20, LIGHTGRAY);
    } else if (estadoAtual == TELA_GAME_OVER) {
        Rectangle origemFundo = { 0, 0, (float)assets.fundoJogo.width, (float)assets.fundoJogo.height };
        Rectangle destinoFundo = { 0, 0, (float)screenW, (float)screenH };
        DrawTexturePro(assets.fundoJogo, origemFundo, destinoFundo, (Vector2){0, 0}, 0.0f, WHITE);

        DrawRectangle(0, 0, screenW, screenH, (Color){ 40, 10, 10, 230 });

        DrawTextCentered("TEMPO ESGOTADO!", screenH / 3, 40, RED);
        DrawTextCentered(TextFormat("Você fez %d de %d pontos.", player.pontosTotais, PONTOS_PARA_VENCER), screenH / 2 - 20, 22, WHITE);
        DrawTextCentered("Pressione R para tentar novamente", screenH / 2 + 50, 20, LIGHTGRAY);
        DrawTextCentered("Pressione M para voltar ao menu", screenH / 2 + 90, 20, LIGHTGRAY);
    } else if (estadoAtual == TELA_INVENTARIO) {
        DrawTextCentered("INVENTÁRIO", 140, 30, BLACK);
        DrawTextCentered("Tela em construcao", screenH / 2, 20, GRAY);
        DrawTextCentered("Pressione M para voltar", screenH - 60, 20, GRAY);
    } else if (estadoAtual == TELA_CATALOGO) {
        DrawTextCentered("CATÁLOGO", 140, 30, BLACK);
        DrawTextCentered("Tela em construcao", screenH / 2, 20, GRAY);
        DrawTextCentered("Pressione M para voltar", screenH - 60, 20, GRAY);
    } else if (estadoAtual == TELA_CREDITOS) {
        DrawTextCentered("BCC - IFSP - Campus Sao Joao da Boa Vista", 120, 22, BLACK);
        DrawTextCentered("INTEGRANTES:", 180, 22, MAROON);
        
        DrawTextCentered("FERNANDA CHEN", 220, 20, BLACK);
        DrawTextCentered("GLEYCE KELLY", 250, 20, BLACK);
        DrawTextCentered("MILENA CRISTINA", 280, 20, BLACK);
        DrawTextCentered("SOFIA DE BARROS", 310, 20, BLACK);
        DrawTextCentered("YONÁ MAIA", 340, 20, BLACK);

        DrawTextCentered("Pressione M para voltar", screenH - 60, 20, GRAY);
    }

    EndDrawing();
}