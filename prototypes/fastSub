#include <stdio.h>
#include <stdlib.h>

// Divisions rapides sans multiplication basées sur le shift-add consommant entre 9 et 11 cycles

int fastSub3(int a){        // Ok pour tout nombre inférieur à 402 653 184, 9 cyles sur ARM
    a  = (a <<  2) + a;
    a += (a >>  4) + 1;
    a += (a >>  8) + 1;
    a += (a >> 16) + 1;

    return a >> 4;
}

int fastSub5(int a){        // Ok pour tout nombre inférieur à 671 088 640, 9 cyles sur ARM
    a += (a <<  1) + 1;
    a += (a >>  4) + 1;
    a += (a >>  8) + 1;
    a += (a >> 16);

    return a >> 4;
}

int fastSub6(int a){        // Ok pour tout nombre inférieur à 805 306 368, 11 cyles sur ARM
    a  = (a <<  1);
    a += (a >>  2) + 1;
    a += (a >>  4) + 1;
    a += (a >>  8) + 1;
    a += (a >> 16);

    return a >> 4;
}

int fastSub7(int a){        // Ok pour tout nombre inférieur à 234 881 024, 8 cyles sur ARM
    a  = (a <<  3) + a;
    a += (a >>  6) + 1;
    a += (a >> 12) + 1;
    a += (a >> 24);

    return a >> 6;
}

int fastSub9(int a){        // Ok pour tout nombre inférieur à 301 989 888, 8 cyles sur ARM
    a  = (a <<  3) - a;
    a += (a >>  6) + 1;
    a += (a >> 12) + 1;
    a += (a >> 24);

    return a >> 6;
}

int fastSub10(int a){       // Ok pour tout nombre inférieur à 357 872 980, 8 cyles sur ARM
    a += (a >>  1) + 1;
    a += (a >>  4) + 1;
    a += (a >>  8);
    a += (a >> 16);

    return a >> 4;
}

int fastSub15(int a){       // Ok pour tout nombre inférieur à 505 290 270, 9 cyles sur ARM
    a  = (a <<  1) + ((a + 1) >>  3);
    a += (a >>  8) + 1;
    a += (a >> 16) + 1;

    return a >> 5;
}

int main()
{
    unsigned int x = 10000;

    for(unsigned int y = 0; y < x; y++){
        if(fastSub3(y) != (y / 3)){
            printf("%d / 3 = %d\n", y, fastSub3(y));
            y = x;
        }
    }

    printf("Fin !\n");
    return 0;
}
