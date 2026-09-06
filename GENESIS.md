# Genèse du Projet & Philosophie d'Ingénierie

## Introduction
Dans les années 90, la programmation est un domaine obscur et difficile d'accès, surtout pour un adolescent sans connexion dans le domaine. Faute de réussir à trouver ma voie là-dedans, je me suis orienté vers un cursus professionnel en électronique qui m'a occupé 5 ans durant les années 2000. C'est pendant cette période que j'ai fait la découverte de RPG Maker 2000 et de sa communauté très versée dans la customisation des mécaniques de jeux. La démocratisation d'Internet m'aura finalement permis en 2008 d'apprendre le PHP, qui sera quelques années plus tard le pont vers le C.

---

## L'électronique : la discipline du crayon et du papier
Travailler avec des composants discrets c'est avant tout l'école du gratte-papier. Un domaine où il est impératif de poser son circuit sur le papier et de tout recontrôler avant la mise sous tension, sous peine de finir avec un condensateur dans l’œil. Il est nécessaire de penser en amont pour éviter les problèmes en aval, c'est une discipline qui s'applique particulièrement bien au monde de la programmation bas niveau.

Même avec cette rigueur, l'électronique impose de travailler avec des composants imparfaits possédant des tolérances et des plages de fonctionnement. Mon professeur d'électronique en BEP aimait nous rappeler la "Loi de l'emmerdement maximal" (loi de Murphy) à travers les pannes fantômes : ces dysfonctionnements inexplicables causés par la dérive thermique et le cumul des tolérances de composants analogiques. C'est dans ce contexte que j'ai découvert l'Algèbre de Boole, un domaine imperméable au bruit et aux approximations, où $1 \text{ AND } 0$ vaut $0$, de manière exacte, déterministe et absolue.

---

## RPG Maker : un laboratoire sous contrainte
À la grande époque de RPG Maker 2000, on voyait souvent des concours techniques ou créatifs. L'un d'entre eux m'a particulièrement marqué : une démo qui transformait un RPG en vue du dessus avec déplacements case par case en un véritable jeu de plateformes au pixel près. RPG Maker ne fonctionnant qu'avec des événements rigides et extrêmement limités, le seul moyen de parvenir à un tel résultat résidait dans un agencement logique et structurel extrêmement bien pensé. C'est devenu ma principale obsession.

À force de travail sur le papier, j'ai réussi à reproduire ce système. En épurant la structure au maximum, les conditions ont commencé à fondre pour devenir extrêmement rares. RPG Maker VX Ace, bien que plus évolué que son ancêtre, tolère très mal les cascades de conditions imbriquées et les boucles lourdes. La leçon a été immédiate : plus le code est court, direct et plat, plus l'exécution est fluide à l'écran.

---

## Le langage C : du software au hardware
Le tournant s'opère en 2022 avec la découverte des opérateurs bit à bit en C. En travaillant sur les additions, le lien avec l'Algèbre de Boole est devenu une évidence : coder en C revient à créer des circuits virtuels. En approfondissant les opérations sur deux variables, il est devenu possible de les comparer ou de les piloter avec des masques, exactement comme on le ferait avec des conditions. Les dernières conditions subsistant dans mon moteur sur RPG Maker pouvaient désormais être intégralement remplacées par des opérateurs bit à bit.

Pour confirmer qu'il était possible de se passer totalement de conditions, je me suis lancé dans la conception d'un jeu de Morpion tenant sur un unique registre 32 bits, entièrement *branchless*. Depuis, l'utilisation du *bitwise* s'accompagne systématiquement d'un calcul de cycles et d'une recherche permanente d'optimisation des ressources de la machine.

---

## L'Émergence par la contrainte
La logique binaire est un vocabulaire minuscule d'une dizaine d'opérateurs. Trop se documenter à l'avance, c'est s'enfermer dans les solutions des autres et se couper de l'émergence naturelle des règles booléennes. Les *Bit Hacks* de Stanford ne sont pas un manuel d'instructions à copier, mais la preuve vivante que sous chaque problème complexe se cache une solution déterministe exacte.
