#include <stdio.h>
#include <stdlib.h>

/****************************************************************************
 * ARCHIVE DE LABORATOIRE / BANC D'ESSAI (2026)                             *
 *                                                                          *
 * Ce fichier regroupe mes premières explorations sur :                     *
 * - Primitives logiques sans branchement (isEqual, isMax, isNotZero)       *
 * - Additionneur/Soustracteur binaire matériel (bitAdd, bitSub)            *
 * - Logarithme base 2 et division via séquence de De Bruijn (0x07C4ACDD)   *
 * - Prototype initial de Tic-Tac-Toe condensé sur registre 32 bits         *
 *                                                                          *
 ****************************************************************************/

/************************************************
* FONCTIONS FONDAMENTALES                       *
* Ne repose sur aucune fonction supplémentaire  *
*************************************************/

int isEqual(int a, int b){
    a ^= b;

    a |= a >> 1;
    a |= a >> 2;
    a |= a >> 4;
    a |= a >> 8;
    a |= a >> 16;

    return (~a & 1);
}

int isMax(int a, int b){
    int c = a;
    a = (a ^ b) & a;
    b = (c ^ b) & b;

    b |= b >> 1;
    b |= b >> 2;
    b |= b >> 4;
    b |= b >> 8;
    b |= b >> 16;

    a = (a ^ b) & a;

    a |= a >> 1;
    a |= a >> 2;
    a |= a >> 4;
    a |= a >> 8;
    a |= a >> 16;

    return (a & 1);
}

int isNotZero(unsigned int a){
    a |= a >> 16;
    a |= a >> 8;
    a |= a >> 4;
    a |= a >> 2;
    a |= a >> 1;

    return a & 1;
}

/****************************************************************************
 * ROTATION DE BITS (ROTATE RIGHT / ROR)                                    *
 * Essai d'implémentation manuelle d'une rotation de 1 bit vers la droite.  *
 *                                                                          *
 * a = ((a & 1) << 30) ^ (a >> 1);  // Pour variable signée                 *
 *                                                                          *
 * a = (a << 31) ^ (a >> 1);        // Pour variable non signée             *
 *                                                                          *
 * Formule canonique en C (reconnue et traduite en 1 cycle ROR par GCC/ARM) *
 * uint32_t ror1(uint32_t val) { return (val >> 1) | (val << 31); }         *
 ****************************************************************************/

/************************************************
* FONCTIONS PRINCIPALES                         *
* Repose sur les fonctions fondamentales        *
*************************************************/

int isPositive(int a){
    return ((a  >> 31) ^ 1) & isNotZero(a) ;
}

int bitAdd(int a, int b){
    int c;

    do {
        c = a;
        a = c ^ b;
        b = (c & b) << 1;
    } while(isNotZero(b));

    return a;
}

int bitSub(int a, int b){
    int c;

    do {
        c = a;
        a = c ^ b;
        b = (~(c) & b) << 1;
    } while(isMax(b, 0));

    return a;
}

int multiplication(int a, int b){
    int s;

    do {
        if(b & 1){
            s = bitAdd(a, s);
        }
        a <<= 1;
        b >>= 1;
    } while(isNotZero(b));

    return s;
}

int logBase2(int a){
    // Méthode moderne
    // a = __builtin_clz(a) ^ 31;
    int tab32[32] = {
     0,  9,  1, 10, 13, 21,  2, 29,
    11, 14, 16, 18, 22, 25,  3, 30,
     8, 12, 20, 28, 15, 17, 24,  7,
    19, 27, 23,  6, 26,  5,  4, 31};

    a |= a >> 1;
    a |= a >> 2;
    a |= a >> 4;
    a |= a >> 8;
    a |= a >> 16;

    return tab32[(a * 0x07C4ACDDU) >> 27];
}

int division(int a, int b){
    int m = 0; // MSB
    int c = 0; // Error
    int s = 0; // Result
    int tab32[32] = {
     0,  9,  1, 10, 13, 21,  2, 29,
    11, 14, 16, 18, 22, 25,  3, 30,
     8, 12, 20, 28, 15, 17, 24,  7,
    19, 27, 23,  6, 26,  5,  4, 31};

    // On calcule logBase2(b) + 1
    b |= b >> 1;
    b |= b >> 2;
    b |= b >> 4;
    b |= b >> 8;
    b |= b >> 16;

    b = ((b << 1) ^ (b ^ (1u << 0)));

    m = tab32[(b * 0x07C4ACDDU) >> 27];

    // Boucle principale du script
    while(a >= b){
        // Correction
        c = (a >> m) | isEqual((a >> m ), 0);
        // Division
        s += c;
        a -= c * b;
    }
}

int main()
{
    /** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * MORPION INTEGER                                                                     *
    * Création d'un jeu de TIC TAC TOE avec une variable si possible et l'utilisation a   *
    * maxima des opérateurs logiques, ou Bitwise.                                         *
    *** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    unsigned int myGame = 7 << 9;
    // 31 30 29 28 | 27 26 25 24 | 23 22 21 20 | 19 18 17 16 | 15 14 13 12 | 11 10 09 08 | 07 06 05 04 | 03 02 01 00
    //  0  0  0  0 |  0  0  0  0 |  0  0  0  0 |  0  0  0  0 |  0  0  0  0 |  0  0  0  0 |  0  0  0  0 |  0  0  0  0
    // Bit 00 à 17 -> Représentation de la grille de jeu
    // Bit 18      -> Status de la partie en cours
    // Bit 19      -> Joueur actif (0 = J2, 1 = J1)

    // Inversion du joueur actif et des grilles en mémoire
    myGame ^= 1U << 19;
    myGame = (myGame & 0xFFFC0000) | ((myGame & 0x0003FE00) >> 9) | ((myGame & 0x000001FF) << 9);

    // Saisie du coup


    // Vérification des solutions et mise à jour du status de la partie
    myGame |= (((myGame & 0x00000001) << 18) & ((myGame & 0x00000002) << 17) & ((myGame & 0x00000004) << 16)) |
              (((myGame & 0x00000008) << 15) & ((myGame & 0x00000010) << 14) & ((myGame & 0x00000020) << 13)) |
              (((myGame & 0x00000040) << 12) & ((myGame & 0x00000080) << 11) & ((myGame & 0x00000100) << 10)) |
              (((myGame & 0x00000001) << 18) & ((myGame & 0x00000008) << 15) & ((myGame & 0x00000040) << 12)) |
              (((myGame & 0x00000002) << 17) & ((myGame & 0x00000010) << 14) & ((myGame & 0x00000080) << 11)) |
              (((myGame & 0x00000004) << 16) & ((myGame & 0x00000020) << 13) & ((myGame & 0x00000100) << 10)) |
              (((myGame & 0x00000001) << 18) & ((myGame & 0x00000010) << 14) & ((myGame & 0x00000100) << 10)) |
              (((myGame & 0x00000004) << 16) & ((myGame & 0x00000010) << 14) & ((myGame & 0x00000040) << 12)) |
              isEqual((((myGame & 0x0003FE00) >> 9) | (myGame & 0x000001FF)), 0x000001FF) << 18;

    // Condition de fin de partie
    if((myGame & (1U << 18)) >> 18){
        printf("Fin de partie.\n");
    }

    return 0;
}
