#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_ATAQUES 12
#define MAX_HISTORICO 17
#define INVALIDO -1
#define NUM_MOVES 5
#define NUM_COMBOS 4
#define NUM_CHEATS 5

#define INIT_VIDA 837
#define INIT_STAM 791

#define MAX_VIDA 999
#define MAX_STAM 1003

#define STAM_LOSS 23

typedef struct
{
    int id;
    int value;
} CheatCode;

typedef struct No
{
    int value;
    struct No *anterior;
} No;

typedef struct
{
    int id;
    No *vida;
    No *estamina;
    No *ataques;
    int totalAtaques;
    int multiplier;
} Jogador;

enum Ataques
{
    ZARABATANA,
    PONTAPE,
    AGARRAR,
    ESTALADA,
    TOMBETA,
    RASTEIRA,
    COTOVELADA,
    BICADA,
    ONDA_DE_CHOQUE,
    MURRO,
    DEFENDER,
    DESCANSAR
};

int matrizEfeitos[NUM_ATAQUES][NUM_ATAQUES] = {
    {0, 4, 8, 12, 16, -20, -16, 12, -8, -4, 0, 20},
    {-4, 0, 4, 8, 12, 16, -20, -16, -12, 16, 0, 20},
    {-8, -4, 0, 4, 8, 12, 16, -20, -16, 12, 0, 20},
    {-12, -8, -4, 0, 4, -8, 12, 16, 20, -16, 0, 20},
    {-16, -12, -8, -4, 0, 4, -8, 12, 16, 20, 0, 20},
    {20, -16, -12, 8, -4, 0, 4, 8, 12, -16, 0, 20},
    {16, 20, -16, -12, 8, -4, 0, -8, -4, -8, 0, 20},
    {-12, 16, 20, -16, -12, -8, 8, 0, 16, -20, 0, 20},
    {8, 12, 16, -20, -16, -12, 4, -16, 0, 20, 0, 20},
    {4, -16, -12, 16, -20, 16, 8, 20, -20, 0, 0, 20},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {-20, -20, -20, -20, -20, -20, -20, -20, -20, -20, 0, 0}};

int readAttack(char *entrada, const char *attackNames[NUM_ATAQUES])
{
    for (int i = 0; i < NUM_ATAQUES; i++)
        if (strcmp(entrada, attackNames[i]) == 0)
            return i;
    return INVALIDO;
}

void addNo(No **head, int value)
{
    No *novo = malloc(sizeof(No));
    novo->value = value;
    novo->anterior = *head;
    *head = novo;
}

void initPlayer(Jogador *j, int numJogador)
{
    j->id = numJogador;
    j->vida = j->estamina = NULL;
    addNo(&j->vida, INIT_VIDA);
    addNo(&j->estamina, INIT_STAM);
    j->ataques = NULL;
    j->totalAtaques = 0;
    j->multiplier = 1;
}

void addLife(Jogador *j, int resultado)
{
    int novaVida = 0;
    resultado = resultado >= 200 ? resultado : resultado * j->multiplier;
    novaVida = j->vida->value - resultado;
    addNo(&j->vida, novaVida > MAX_VIDA ? MAX_VIDA : novaVida);
}

void addStamina(Jogador *j, int value)
{
    int novaEstamina = j->estamina->value - value;
    addNo(&j->estamina, novaEstamina > MAX_STAM ? MAX_STAM : novaEstamina < 0 ? 0
                                                                              : novaEstamina);
}

void addAttack(Jogador *j, int ataqueId)
{
    addNo(&j->ataques, ataqueId);
    j->totalAtaques++;
}

void imprimirHistorico(Jogador *j, const char *combos[], const char *attackNames[NUM_ATAQUES])
{
    int historico[MAX_HISTORICO], count = 0;
    No *current = j->ataques;
    printf("H: ");
    while (current && count < MAX_HISTORICO)
    {
        historico[count++] = current->value;
        current = current->anterior;
    }
    while (count)
        printf("%s",
               historico[--count] < 200 ? attackNames[historico[count]] : combos[(historico[count] / 100) - 2]);
    printf("\n");
}

void setMultiplier(Jogador *j)
{
    int est = j->estamina->value;
    j->multiplier = est > 750 ? 1 : est > 500 ? 2
                                : est > 250   ? 3
                                              : 4;
}

void printStatus(Jogador *j, const char *combos[], const char *attackNames[NUM_ATAQUES])
{
    setMultiplier(j);
    printf("P#%d [%d|%d] (x%d)\n", j->id, j->vida->value, j->estamina->value, j->multiplier);
    imprimirHistorico(j, combos, attackNames);
}

int checkTarzan(char *entrada)
{
    int numero;
    char prefixo[15];
    if (sscanf(entrada, "%13s%d", prefixo, &numero) == 2 && strcmp(prefixo, "TARZANTABORDA") == 0)
        return numero;
    return INVALIDO;
}

int checkCombo(char input[], const char *combos[])
{
    for (int i = 0; i < NUM_COMBOS; i++)
        if (strcmp(input, combos[i]) == 0)
            return (i + 2) * 100;
    return INVALIDO;
}

CheatCode checkCheat(char input[], const char *cheats[])
{
    // Se não achar um cheatcode, usa-se esse
    CheatCode result = {-1, 0};

    // Verifica se é o Modo-Jesus
    if (strcmp(input, cheats[0]) == 0)
    {
        result.id = 0;
        result.value = 0;
        return result;
    }

    // Se não for, procurar pelos outros cheat codes
    for (int i = 1; i < NUM_CHEATS; i++)
    {
        char *prefixo = strstr(input, cheats[i]);
        if (prefixo != NULL)
        {
            int numero;
            if (sscanf(input + strlen(cheats[i]), "%d", &numero) == 1)
            {
                result.id = i;
                result.value = numero;
                return result;
            }
        }
    }

    return result;
}

void retrocederAtaques(Jogador *j, int numAtaques)
{
    while (numAtaques-- && j->vida->anterior)
    {
        No *tempVida = j->vida, *tempAtaque = j->ataques, *tempEstamina = j->estamina;
        j->vida = j->vida->anterior;
        j->ataques = j->ataques->anterior;
        j->estamina = j->estamina->anterior;
        free(tempVida);
        free(tempAtaque);
        free(tempEstamina);
        j->totalAtaques--;
    }
}

void backInTime(int numTurns, Jogador *jogadores[2], int nGolpes[2])
{
    printf(">>> Voltou %d ataques", numTurns);
    for (int i = 0; i < 2; i++)
    {
        retrocederAtaques(jogadores[i], numTurns);
        nGolpes[i] = 0;
    }
}

void resetGolpes(int golpes[2][NUM_MOVES], int nGolpes[2])
{
    for (int i = 0; i < NUM_MOVES; i++)
        golpes[0][i] = golpes[1][i] = DESCANSAR;
    nGolpes[0] = nGolpes[1] = 0;
}

void getInput(char entrada[50])
{
    printf("I: ");
    scanf("%49s", entrada);
}

void getRandomInput(char entrada[50], const char *combos[], const char *attackNames[])
{
    // Copia local do array sem o " "
    const char *localAttackNames[NUM_ATAQUES - 1];
    for (int i = 0; i < NUM_ATAQUES - 1; i++)
    {
        localAttackNames[i] = attackNames[i];
    }

    printf("I: ");

    // numero aleatorio entre 1 e 20
    int randomNumber = rand() % 20 + 1;

    // Usa o numero para determinar a acao
    if (randomNumber <= 12) // ataques normais
    {
        entrada[0] = '\0';
        for (int i = 0; i < NUM_MOVES; i++)
        {
            strcat(entrada, localAttackNames[rand() % (NUM_ATAQUES - 1)]);
        }
    }
    else if (randomNumber <= 16) // Combo aleatorio
    {
        strcpy(entrada, combos[randomNumber - 13]);
    }
    /*else if (randomNumber <= 19) // Cheat-code aleatorio
    {
        int cheatIndex = rand() % 5;
        strcpy(entrada, cheats[cheatIndex]);
        if (cheatIndex != 0) // Se não for o Modo-Jesus
        {
            // Adiciona um numero (MAX_STAM ou MAX_VIDA, depende do cheat code)
            char numStr[10];
            if (cheatIndex == 1 || cheatIndex == 2)
            {
                sprintf(numStr, "%d", rand() % MAX_STAM + 1);
            }
            else
            {
                sprintf(numStr, "%d", rand() % MAX_VIDA + 1);
            }
            strcat(entrada, numStr);
        }
    }*/
    else // Somente no 17 usa o "TARZANTABORDA"
    {
        strcpy(entrada, "TARZANTABORDA");
        char numStr[10];
        sprintf(numStr, " %d", rand() % 17 + 1); // entre 1 e 17 turnos
        strcat(entrada, numStr);
    }
    printf("%s\n", entrada);
}

int getAttacks(char entrada[50], Jogador *j, int nGolpes[2], int golpes[2][NUM_MOVES], const char *attackNames[NUM_ATAQUES])
{
    int len = strlen(entrada);
    if (len > NUM_MOVES)
    {
        printf("Entrada invalida\n");
        exit(0);
    }
    nGolpes[j->id - 1] = len;
    for (int i = 0; i < len; i++)
    {
        int ataqueId = readAttack((char[]){entrada[i], '\0'}, attackNames);
        if (ataqueId != INVALIDO)
        {
            golpes[j->id - 1][i] = ataqueId;
        }
        else
        {
            printf("Entrada invalida\n");
            exit(0);
        }
    }
    return 1;
}

void remove_newline(char *str)
{
    size_t len = strlen(str);
    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
    {
        str[len - 1] = '\0';
        if (len > 1 && (str[len - 2] == '\r' || str[len - 2] == '\n'))
        {
            str[len - 2] = '\0';
        }
    }
}

int getInputFromFile(char entrada[50], FILE *file)
{
    printf("I: \n");
    if (fgets(entrada, 50, file) != NULL)
    {
        remove_newline(entrada);
        return 1;
    }
    return 0;
}

int temCombo(int attP1, int attP2)
{
    return attP1 >= 200 || attP2 >= 200;
}

void applyCheatCode(CheatCode cheat, Jogador *jogadores[2])
{
    int newValue = 0;
    switch (cheat.id)
    {
    // Modo-Jesus
    case 0:
        retrocederAtaques(jogadores[0], 999); // 999 eh um numero magico para voltar ao comeco do jogo
        retrocederAtaques(jogadores[1], 999);
        break;
    case 1: // Alt-F4
        newValue = jogadores[0]->estamina->value + cheat.value;
        jogadores[0]->estamina->value = newValue > MAX_STAM ? MAX_STAM : newValue;
        break;
    case 2: // Kebab
        newValue = jogadores[1]->estamina->value + cheat.value;
        jogadores[1]->estamina->value = newValue > MAX_STAM ? MAX_STAM : newValue;
        break;
    case 3: // Hiroshima
        newValue = jogadores[0]->vida->value + cheat.value;
        jogadores[0]->vida->value = newValue > MAX_VIDA ? MAX_VIDA : newValue;
        break;
    case 4: // Nood-Mode
        newValue = jogadores[1]->vida->value + cheat.value;
        jogadores[1]->vida->value = newValue > MAX_VIDA ? MAX_VIDA : newValue;
        break;
    default:
        break;
    }
}

void freeList(No *head)
{
    No *current = head;
    while (current != NULL)
    {
        No *next = current->anterior;
        free(current);
        current = next;
    }
}

int main(int argc, char *argv[])
{
    Jogador jogador1, jogador2;
    int golpes[2][NUM_MOVES], nGolpes[2], turn = 0;
    char entrada[50];
    bool jogoAtivo = true;
    Jogador *jogadores[2] = {&jogador1, &jogador2}, *jogadorAtual;
    FILE *file = NULL;
    bool fileInput = false;
    const char *combos[] = {"TATAPAAA", "STTEACC", "DADBAD", "ARROZAO"};
    const char *cheats[] = {"Modo-Jesus", "Alt-F4", "Kebab", "Hiroshima", "Nood-Mode"};
    const char *attackNames[NUM_ATAQUES] = {"Z", "P", "A", "E", "T", "R", "C", "B", "O", "M", "D", " "};
    int attsBack = 0, resultado = 0;
    CheatCode cheatCode = {0, 0};
    int comboId = 0;
    int maxGolpes = 0;
    int attP1 = 0, attP2 = 0;
    int dStamP1 = 0, dStamP2 = 0, dLifeP1 = 0, dLifeP2 = 0;
    const char *p1Attack, *p2Attack;

    initPlayer(&jogador1, 1);
    initPlayer(&jogador2, 2);

    if (argc > 1)
    {
        srand(atoi(argv[1]));    
    }
    else
        srand(1); // Seed de defeito é 1

    if (argc > 2)
    {
        file = fopen(argv[2], "r");
        if (file != NULL)
        {
            fileInput = true;
        }
        else
        {
            printf("Erro ao abrir o arquivo %s\n", argv[1]);
            return 1;
        }
    }

    while (jogoAtivo)
    {
        resetGolpes(golpes, nGolpes);
        while (turn < 2)
        {
            jogadorAtual = jogadores[turn];
            printStatus(jogadorAtual, combos, attackNames);

            if (jogadorAtual == &jogador1)
            {
                if (fileInput)
                {
                    if (!getInputFromFile(entrada, file))
                    {
                        jogoAtivo = false;
                        break;
                    }
                }
                else
                {
                    getInput(entrada);
                }
            }
            else
            {
                getRandomInput(entrada, combos, attackNames);
            }

            cheatCode = checkCheat(entrada, cheats);
            if (cheatCode.id != INVALIDO)
            {
                applyCheatCode(cheatCode, jogadores);
                continue;
            }

            attsBack = checkTarzan(entrada);
            if (attsBack != INVALIDO)
            {
                if (jogadorAtual->estamina->value > 500 && jogadorAtual->estamina->value < 900)
                {
                    backInTime(attsBack, jogadores, nGolpes);
                    break;
                }
                else
                {
                    printf("Estamina insuficiente\n");
                    continue;
                }
            }

            comboId = checkCombo(entrada, combos);
            if (comboId != INVALIDO)
            {
                if (jogadorAtual->estamina->value > 750)
                {
                    golpes[jogadorAtual->id - 1][0] = comboId;
                    nGolpes[jogadorAtual->id - 1] = 1;
                }
                else
                {
                    printf("Estamina insuficiente\n");
                    continue;
                }
            }
            else
            {
                if (getAttacks(entrada, jogadorAtual, nGolpes, golpes, attackNames) == INVALIDO)
                    continue;
            }
            turn++;
        }
        turn = 0;

        maxGolpes = nGolpes[0] > nGolpes[1] ? nGolpes[0] : nGolpes[1];

        attP1 = golpes[0][0];
        attP2 = golpes[1][0];
        if (temCombo(attP1, attP2))
        {
            maxGolpes = 1;
        }

        for (int i = 0; i < maxGolpes; i++)
        {
            attP1 = golpes[0][i];
            attP2 = golpes[1][i];
            dStamP1 = 0;
            dStamP2 = 0;
            dLifeP1 = 0;
            dLifeP2 = 0;
            if (temCombo(attP1, attP2))
            {
                dLifeP2 += (attP1 >= 200) ? attP1 : 0;      // se for combo, causa o dano do combo, caso contrário, DESCANSAR.
                dStamP1 = (attP1 >= 200) ? attP1 : -25;     // se for combo, gasta a estamina do combo, caso contrario, DESCANSAR
                attP1 = (attP1 >= 200) ? attP1 : DESCANSAR; // se o ataque não for um combo, então deve virar um DESCANSAR

                dLifeP1 += (attP2 >= 200) ? attP2 : 0;
                dStamP2 = (attP2 >= 200) ? attP2 : -25;
                attP2 = (attP2 >= 200) ? attP2 : DESCANSAR;

                addLife(&jogador1, dLifeP1);
                addLife(&jogador2, dLifeP2);
            }

            if (!(temCombo(attP1, attP2)))
            {
                dStamP1 += attP1 == DEFENDER ? -7 : attP1 == DESCANSAR ? -25
                                                                       : STAM_LOSS;
                dStamP2 += attP2 == DEFENDER ? -7 : attP2 == DESCANSAR ? -25
                                                                       : STAM_LOSS;
                dLifeP1 += attP1 == DEFENDER ? -13 : 0;
                dLifeP2 += attP2 == DEFENDER ? -13 : 0;
                resultado = matrizEfeitos[attP1][attP2];
                addLife(&jogador1, dLifeP1 + (resultado < 0 ? -resultado : 0));
                addLife(&jogador2, dLifeP2 + (resultado > 0 ? resultado : 0));
            }

            p1Attack = attP1 >= 200 ? combos[(attP1 / 100) - 2] : attackNames[attP1];
            p2Attack = attP2 >= 200 ? combos[(attP2 / 100) - 2] : attackNames[attP2];

            addAttack(&jogador1, attP1);
            addStamina(&jogador1, dStamP1);
            addAttack(&jogador2, attP2);
            addStamina(&jogador2, dStamP2);
            printf("[%s,%s]", p1Attack, p2Attack);
        }
        printf("\n");

        if (jogador1.vida->value <= 0 || jogador2.vida->value <= 0)
        {
            // No fluxo normal do jogo, nao ha empates. São possiveis com os cheat codes.
            printf(jogador1.vida->value == 0 && jogador2.vida->value == 0 ? "Empate!\n" : "Jogador %d venceu o jogo!\n", jogador1.vida->value > 0 ? 1 : 2);
            jogoAtivo = false;
        }
    }

    if (fileInput)
    {
        fclose(file);
    }

    freeList(jogador1.vida);
    freeList(jogador1.estamina);
    freeList(jogador1.ataques);
    freeList(jogador2.vida);
    freeList(jogador2.estamina);
    freeList(jogador2.ataques);

    return 0;
}
