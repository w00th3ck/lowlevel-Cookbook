#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main()
{
    /*** REGISTRE MAP 32 BITS
    / 31 30 29 28 27 26 25 24 | 23 22 21 20 19 18 17 16 | 15 14 13 12 11 10 09 08 | 07 06 05 04 03 02 01 00   /
    /        ID TILES 1       |        ID TILES 2       |        ID TILES 3       |        ID TILES 1         /
    /** ******************************************************************************************************/

    uint32_t map_screen[38];

    /*** REGISTRE TUILES 32 BITS
    / 31 30 29 28 27 26 25 24 | 23 22 21 20 19 18 17 16 | 15 14 13 12 11 10 | 09 08 07 06 05 04 | 03 02 01 00 /
    /          FORCE          |          RESERVE        |     EPAISSEUR     |      ORIGINE      |    PENTE    /
    /  VALEUR EN SOUS-PIXELS  |                         |  VALEUR EN PIXELS |  VALEUR EN PIXELS |   DECALAGE  /
    /** ******************************************************************************************************/

    uint32_t tiles_properties[256];

    /*** REGISTRE POSITION 32 BITS
    / 31 30 29 28 27 26 25 24 | 23 22 21 20 19 18 17 16 | 15 14 13 12 11 10 09 08 | 07 06 05 04 03 02 01 00   /
    /     TILES POSITION Y    |   LOCALE POSITION Y     |     TILES POSITION X    |   LOCALE POSITION X       /
    /     VALEUR EN TUILES    |  VALEUR EN SOUS-PIXELS  |     VALEUR EN TUILES    |  VALEUR EN SOUS-PIXELS    /
    /** ******************************************************************************************************/

    /*** REGISTRE PHYSIQUE 32 BITS
    / 31 30 29 28 27 26 25 24 | 23 22 21 20 19 18 | 17 16 15 14 13 | 12 11 10 09 08 | 07 06 05 04 03 02 01 00 /
    /        VELOCITY Y       |       FLAGS       |    HITBOX H    |    HITBOX W    |        VELOCITY X       /
    /  VALEUR EN SOUS-PIXELS  |                   |VALEUR EN PIXELS|VALEUR EN PIXELS|  VALEUR EN SOUS-PIXELS  /
    /** ******************************************************************************************************/

    uint32_t reg_pos = 0;
    uint32_t reg_phys = 0;

    // Extraction des données depuis le registre
    uint32_t pos_x = reg_pos & 0x0000FFFFU;
    uint32_t pos_y = reg_pos >> 16;
    const uint8_t *map = (const uint8_t *)map_screen;

    /***
    / GESTION DES COLLISIONS HORIZONTALES
    / On utilise une logique binaire passant/bloquant sur cette axe.
    /*******************************************************************/
    {   // Extraction de vel_x avec extension de signe directe sur 32 bits
        int32_t vel_x = ((int32_t)reg_phys << 24) >> 24;
        // Extraction et conversion de box_h en sous-pixels
        uint32_t box_w = ((reg_phys & 0x00001F00U) >> 8) << 4;

        // Modification de la position relative de l'origine de l'objet et actualisation de la position
        pos_x += box_w & ~(uint32_t)(vel_x >> 31);
        pos_x += vel_x;
    }

    {
        // Extraction de la hauteur de la hitbox et conversion en sous-pixels (8.8)
        uint32_t box_h = ((reg_phys & 0x0001FE00U) >> 13) << 4;

        // Coordonnées de tuiles absolues
        uint32_t tile_x  = (pos_x & 0x0000FF00U) >> 8;
        uint32_t tile_y1 = (pos_y & 0x0000FF00U) >> 8;
        uint32_t tile_y2 = ((pos_y + box_h) & 0x0000FF00U) >> 8;

        uint8_t t1 = map[(tile_y1 << 4) + tile_x];
        uint8_t t2 = map[(tile_y2 << 4) + tile_x];

        // Arbitrage et récupération des propriétés dans la LUT
        uint32_t tile = tiles_properties[(t1 > t2) ? t1 : t2];

        // Gestion de la vitesse
        int32_t vel_x = ((int32_t)reg_phys << 24) >> 24;
        vel_x =  vel_x & ((tile & 1U) - 1); // 0 : passant et 1 : bloquant pour gagner un cycle

        // Correction de la pénétration si nécessaire
        int32_t vel_mask = ((~vel_x >> 31) << 1) - 1;
        pos_x = ((pos_x & 0x0000FF00U) + vel_mask) | ((pos_x & 0x000000FFU) & (((tile & 1U) - 1)));
    }

    /***
    / GESTION DES COLLISIONS VERTICALES
    / Contrairement à l'axe X, on va détecter les topographies de terrain nécessitant un asservissement
    / de l'axe Y par l'axe X. Pour ça nous allons utiliser l'indexation des tuiles ainsi que l'ancrage
    / situé sous le personnage plutôt que simplement un système passant/bloquant.
    /*******************************************************************/
    {   // Extraction de vel_y avec extension de signe directe sur 32 bits
        int32_t vel_y = ((int32_t)reg_phys >> 24);
        // Extraction et conversion de box_h en sous-pixels
        uint32_t box_h = (reg_phys & 0x0003E000U) >> 13;

        // Modification de la position relative de l'origine de l'objet et actualisation de la position
        pos_y += box_h & ~(uint32_t)(vel_y >> 31);
        pos_y += (uint32_t)vel_y;
    }

    {
        // Cast du tableau map_screen pour le parcourir octet par octet (tuile par tuile)
        // On déplace le curseur à la bonne ligne du tableau
        const uint8_t *map_row = (const uint8_t *)map_screen + ((pos_y >> 4) & ~0x0FU);

        // Extraction et conversion de box_w en sous-pixels
        uint32_t box_w = (reg_phys & 0x00001F00U) >> 8;

        // Récupération de IDMax
        uint8_t t1 = map_row[pos_x >> 8];
        uint8_t t2 = map_row[(pos_x + box_w) >> 8];
        tile = tiles_properties[(t1 > t2) ? t1 : t2];
    }

    {
        // Extraction des données
        uint32_t relative_x = (pos_x & 0x000000FFU) >> 4;
        uint32_t pente = tile & 0x0000000FU;
        uint32_t origine = (tile & 0x000003F0U) >> 4;
        uint32_t epaisseur = (tile & 0x0000FC00U) >> 10;

        // Formule pour obtenir la position relative_y en fonction de relative_x
        uint32_t correction = origine - ((relative_x << pente) >> 4);
    }

    return 0;
}
