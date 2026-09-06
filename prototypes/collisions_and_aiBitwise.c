#include <stdio.h>
#include <stdlib.h>

int main()
{
    /******************************************************************************
     * ASTUCES BITWISE & MASQUES BASÉS SUR LE SIGNE
     *
     * 1. Modulo d'une puissance de 2, myNumber & (2^n - 1). Modulo 32 ci-dessous :
     *    result = myNumber & 31;
     *
     * 2. Retourne 1 ou -1 en fonction du signe de l'int :
     *    result = ((myNumber >> 31) << 1) + 1; // 32 BITS
     *    result = ((myNumber >> 63) << 1) + 1; // 64 BITS
     *
     * 3. Décalage de Hitbox conditionnel selon la direction (Branchless AABB) :
     *    Évite le branchement conditionnel lors des tests de bordure de tuile.
     *    CaseX = CoordX + VitesseX + (((VitesseX >> 31) - 1) & HitboxX);
     ******************************************************************************/

    /******************************************************************************
     * TRAVAIL THEORIQUE SUR LES PENTES
     *
     * 1. Collision - Détection de collision sur une grille
     *    On utilise les CoordX, CaseX, HitboxX, VitesseX :
     *
     *    CaseX = CoordX + VitesseX + (((VitesseX >> 31) - 1) & HitboxX);
     *    CaseY = CoordY;
     *    EtatCase1 = GameGrid[CaseX >> 5][CaseY >> 5];
     *    CaseY += HitboxY;
     *    EtatCase2 = GameGrid[CaseX >> 5][CaseY >> 5];
     *
     * 2. Collisions sur un sol non plat
     *    On assigne une valeur de pente à une case : 8 indique que l'on monte par exemple de 8 pixels sur une case de 32 pixels
     *    En somme le sol s'élève de 1 pixels tous les 32 / 8 ou 32 >> 3 ou tous les 4 pixels
     *    La position du joueur dans la case permet de renvoyer sa position verticale théorique dans celle-ci grâce au modulo
     *    Si le joueur a une coordonnée X de 27 alors 27 / (32 >> 3) renvoie la valeur verticale acquise grâce au sol, mais ce n'est valable qu'un à un instant t
     *    Pour généraliser le procédé il faut la valeur Y de l'origine de la case.
     *    Si la coordonnées X est cette fois de 122 : ((122 + 1) % 32) / (32 / 8) soit (((122 + 1) % 32) * 8) / 32
     *    En optimisant grâce au puissance de 2 : (((X + 1) & 31) << PENTE) >> 5
     *    Avec X la coordonnée du joueur en abscisse et PENTE la puissance de 2 de la pente. ATTENTION SI LA PENTE EST NULLE LE SCRIPT RENVOIE 1 !
     *    0 1 2 3 | 4 5 6 7 | 8 9 10 11 | 12 13 14 15 | 16 17 18 19 | 20 21 22 23 | 24 25 26 27 | 28 29 30 31
     *    L'élévation se fera de 0 à 7 pixels, la correction +1 ne sert finalement pas à grand chose et peut même induire une erreur
     *
     *    ORIGINE + ((X & 31) << PENTE) >> 5;
    ******************************************************************************/

    int myNumber = 0;
    int monNiveau[6][8] =  {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', '#', '#', ' ', ' ', '#', ' '},
                            {'#', '#', '#', '#', '0', '#', '#', '#'},
                            {'#', '#', '#', '#', '#', '#', '#', '#'}};
    int scrollingOffsetY = 0;
    int scrollingOffsetX = 128;          // La valeur doit respecter les numérotations des cellules des arrays
    int resolutionVerticale = 6;
    int resolutionHorizontale = 128;
    int debutDeRendu = 0;
    int finDeRendu = 0;

    /******************************************************************************
     * TRAVAIL THEORIQUE SUR LE SCROLLING
     * Objectif : faire défiler le niveau sans bouger le personnage.
     *
     * Informations : SDL2 a pour origine des pixels le coin supérieur gauche.
     *
     * Pistes de réflexions :
     *     - Les coordonnées intrinsèques du personnage ne changent jamais, elles sont restreintes par la résolution
     *      - Le décor est indéxé à une variable globale, image du déplacement : scrollingOffsetX
     *      - Si le niveau est découpé en une grille de 32 pixels carrés alors il faut charger une colonne avant et une après en plus de la résolution de l'écran
     *      - scrollingOffsetX doit stocker une donnée en pixel, en l'ajoutant aux coordonnées du personnage on obtient sa position globale sur la map
     *      - Rien de plus pour le moment...
     *
     * Réalisation :
     *      - On génère une grille qui simule notre niveau.
     *      - On ajoute notre variable ScrollingHorizontale et on l'initialise au point de départ souhaité
     *      - On choisit la résolution de notre rendu
     ******************************************************************************/

    for(int i = 0; i <= 5; i++){
        //debutDeRendu = ((scrollingOffsetX - (resolutionHorizontale >> 1)) >> 5) - 1;
        //finDeRendu = debutDeRendu + (resolutionHorizontale >> 5) + 1;
        debutDeRendu = (scrollingOffsetX >> 5) - ((resolutionHorizontale >> 5) >> 1);
        finDeRendu = debutDeRendu + (resolutionHorizontale >> 5);

        for(int j = debutDeRendu; j <= finDeRendu; j++){
            printf("%c", monNiveau[i][j]);
        }
        printf("\n");
    }

    /******************************************************************************
     * TRAVAIL THEORIQUE SUR LA THEORIE DES GRAPHES ET LE BITWISE
     *
     * 1. On simule un Morpion dont les cases sont numérotées de 1 à 9 en ligne de gauche à droite et de bas en haut.
     *    On charge les cases utilisées sur 9 BITS :
     *    0   0 0 0 0   0 0 0 0
     *    9   8 7 6 5   4 3 2 1
     *
     * 2. Les 8 solutions seront les suivantes : 1 2 3, 1 5 9, 1 4 7, 2 5 8, 3 5 7, 3 6 9, 4 5 6, 7 8 9.
     *    La somme des puissances associées :      7  ,  273 ,   73 ,  146 ,   84 ,  292 ,   56 ,  448
     *
     *    int myNumber[2] = {51, 268}; // 49 260
     *    int aiChoices[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
     *
     *    Si le joueur a choisi les cases 5, 1, 6 (49) et l'adversaire 3 et 9 (260)
     *    result = ~(myNumber[0] | (~ 273)) | ~(myNumber[0] | (~ 146)) | ~(myNumber[0] | (~ 84)) | ~(myNumber[0] | (~ 56));
     *
     * 3. Accumulation des solutions
     *    aiChoice[0] += (result & 1);
     *    aiChoice[1] += (result & (1 << 1)) >> 1;
     *    aiChoice[2] += (result & (1 << 2)) >> 2;
     *    aiChoice[3] += (result & (1 << 3)) >> 3;
     *    aiChoice[4] += (result & (1 << 4)) >> 4;
     *    aiChoice[5] += (result & (1 << 5)) >> 5;
     *    aiChoice[6] += (result & (1 << 6)) >> 6;
     *    aiChoice[7] += (result & (1 << 7)) >> 7;
     *    aiChoice[8] += (result & (1 << 8)) >> 8;
     ******************************************************************************/

    return 0;
}
