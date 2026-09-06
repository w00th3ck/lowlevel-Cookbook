#include <stdio.h>
#include <stdlib.h>
#include "engine.h"

typedef void (*FunctionSelector)(unsigned int *);

void GameUpdate(unsigned int *myGame){
    *myGame ^= 1U << 19;
    *myGame = (*myGame & 0xFFFC0000) | ((*myGame & 0x0003FE00) >> 9) | ((*myGame & 0x000001FF) << 9);
}

void DoNothing(unsigned int *myGame){
}

void Ai_Turn(unsigned int *myGame){
    // CODE DE L'IA DU JEU
}

void Human_Turn(unsigned int *myGame) {
    unsigned int move, index;
    // On récupère toutes les cases prises (J1 | J2)
    unsigned int occupied = (*myGame | (*myGame >> 9)) & 0x1FF;

    printf("Coup (1-9) : ");
    if (scanf("%u", &move) != 1) {
        // Gestion de sécurité si l'utilisateur tape une lettre
        while(getchar() != '\n');
        move = 0;
    }

    index = move - 1;

    // 1. On vérifie si l'index est bien entre 0 et 8
    // 2. On vérifie si la case est libre
    unsigned int inRange = (index < 9);
    unsigned int isFree = !((occupied >> (index & 0x1F)) & 1); // 0x1F empêche un décalage infini

    // Le verdict : on ne valide que si les deux sont vrais
    unsigned int success = inRange & isFree;

    // MISE À JOUR DU REGISTRE
    // On écrit le pion seulement si success == 1
    *myGame |= (success << index);

    // MISE À JOUR DU BIT 22 (État du coup)
    // On nettoie d'abord le bit 22, puis on injecte le résultat
    *myGame = (*myGame & ~(1U << 22)) | (success << 22);

    // OPTIONNEL : Stocker l'index du dernier coup (Bits 23-26)
    // Utile pour ton optimisation de calcul de victoire plus tard
    *myGame = (*myGame & ~(0xFU << 23)) | ((index & 0xF) << 23);
}

void Display_Grid(unsigned int g){
    // On identifie qui est le joueur ACTIF (Bit 19)
    unsigned int activeP = (g >> 19) & 1;

    // Macro locale pour calculer le symbole de la case i (0-8)
    // Index 0: '.' | Index 1: 'X' | Index 2: 'O'
    // Cette formule mathématique reste stable malgré le swap des grilles
    #define SYM(i) " .XO"[((g >> i) & 1) * (2 - activeP) + ((g >> (i + 9)) & 1) * (1 + activeP)]

    printf("\n      G R I L L E\n");
    printf("    +---+---+---+\n");
    printf("    | %c | %c | %c |\n", SYM(0), SYM(1), SYM(2));
    printf("    +---+---+---+\n");
    printf("    | %c | %c | %c |\n", SYM(3), SYM(4), SYM(5));
    printf("    +---+---+---+\n");
    printf("    | %c | %c | %c |\n", SYM(6), SYM(7), SYM(8));
    printf("    +---+---+---+\n");

    // Petit affichage du statut (optionnel)
    const char* pName[] = {"JOUEUR 2 (O)", "JOUEUR 1 (X)"};

    // On n'affiche le tour que si le jeu est en cours (Bit 18 == 1)
    if ((g >> 18) & 1) {
        printf("\n ==> Au tour de : %s\n", pName[activeP]);
    } else {
        printf("\n >>> PARTIE TERMINEE <<<\n");
    }

    #undef SYM
}

int isEqual(int a, int b){
    a ^= b;

    a |= a >> 1;
    a |= a >> 2;
    a |= a >> 4;
    a |= a >> 8;
    a |= a >> 16;

    return (~a & 1);
}

int main()
{
    /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * MORPION INTEGER                                                                     *
    * Creating a TIC-TAC-TOE game without loops, branchless and no minimax AI.            *
    *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    FunctionSelector Update[2] = { DoNothing, GameUpdate };
    FunctionSelector InputMode[2] = { Ai_Turn, Human_Turn };

    unsigned int myGame = 7 << 18;
    // 31 30 29 28 | 27 26 25 24 | 23 22 21 20 | 19 18 17 16 | 15 14 13 12 | 11 10 09 08 | 07 06 05 04 | 03 02 01 00
    //  0  0  0  0 |  0  0  0  0 |  0  0  0  0 |  0  0  0  0 |  0  0  0  0 |  0  0  0  0 |  0  0  0  0 |  0  0  0  0
    // BIT00 to 17  -> Game grids
    // BIT18        -> Current game status          (0 = Game over,     1 = Play time)
    // BIT19        -> Active player                (0 = Player2 or AI, 1 = Player1)
    // BIT20        -> Game Mode                    (0 = Single player, 1 = Two players)
    // BIT21        -> Draw                         (0 = No draw,       1 = Draw)
    // BIT22        -> Input Error                  (0 = Error,         1 = Success)

    while((myGame >> 18) & 1){
        // --- STEP 1 : Update the game ---
        // Swapping the active player (BIT 19) and the grids in memory (0-8 <-> 9-17)
        Update[(myGame >> 22) & 1](&myGame);

        // Display
        Display_Grid(myGame);

        // --- STEP 2 : Input ---
        // Select input mode between AI or Human
        InputMode[((myGame >> 19) & 1) | ((myGame >> 20) & 1)](&myGame);

        // --- STEP 3 : Logical Resolution ---
        // Checking solutions and updating the game status (BIT18)
        myGame ^= (((myGame & 0x00000001) << 18) & ((myGame & 0x00000002) << 17) & ((myGame & 0x00000004) << 16)) |
                  (((myGame & 0x00000008) << 15) & ((myGame & 0x00000010) << 14) & ((myGame & 0x00000020) << 13)) |
                  (((myGame & 0x00000040) << 12) & ((myGame & 0x00000080) << 11) & ((myGame & 0x00000100) << 10)) |
                  (((myGame & 0x00000001) << 18) & ((myGame & 0x00000008) << 15) & ((myGame & 0x00000040) << 12)) |
                  (((myGame & 0x00000002) << 17) & ((myGame & 0x00000010) << 14) & ((myGame & 0x00000080) << 11)) |
                  (((myGame & 0x00000004) << 16) & ((myGame & 0x00000020) << 13) & ((myGame & 0x00000100) << 10)) |
                  (((myGame & 0x00000001) << 18) & ((myGame & 0x00000010) << 14) & ((myGame & 0x00000100) << 10)) |
                  (((myGame & 0x00000004) << 16) & ((myGame & 0x00000010) << 14) & ((myGame & 0x00000040) << 12));
        // Checking the draw, if the two grids complete all cells set BIT21 to 1. Triggered by BIT18
        myGame ^= (isEqual(((myGame | (myGame >> 9)) & 0x000001FF), 0x000001FF) & ((myGame >> 18) & 1)) << 21;
        // Change game status when the game is draw
        myGame ^= (myGame & 0x00200000) >> 3;
    }
    // Juste après la sortie du while, on peut afficher le résultat final
    if ((myGame >> 21) & 1) printf("Match Nul !\n");
    else printf("Victoire !\n");

    return 0;
}
