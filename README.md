# Bitwise & Low-Level Engineering Cookbook

> *"Who am I, and what do my algorithms say about me?"*

 Ce recueil documente mes recherches, mes fiches techniques et l'évolution de ma pratique du C bas niveau. L'objectif de cette démarche est de privilégier la compréhension du silicium, le zéro-branchement (*branchless*) et la manipulation directe des registres.

---

##  La Philosophie
- **Algèbre de Boole & Silicium :** Traiter le code non comme une abstraction, mais comme une équation booléenne exécutée par l'ALU.
- **Zero-Branching & Déterminisme :** Éliminer les conditions (`if`, `while`) au profit d'opérateurs logiques (`&`, `|`, `^`, `~`, `<<`, `>>`) pour garantir un temps d'exécution constant.
- **Émergence par la contrainte :** Refuser de consommer des solutions toutes faites pour laisser les propriétés mathématiques émerger de la structure des bits.

 [Lire la Genèse complète du projet dans GENESIS.md](GENESIS.md)

---

##  Index des Recettes

### 01. Les premiers scripts
* [`bitPrimitive_and_gamePrototype`](prototypes/bitPrimitive_and_gamePrototype.c) — Reconstruire la boîte à outils binaire et un prototype de morpion dans un registre unique.
* [`collisions_and_aiBitwise`](prototypes/collisions_and_aiBitwise.c) — Réflexions autour des collisions statiques 2D et sur l'AI du morpion basée sur les graphes.
* [`fastSub`](prototypes/fastSub.c) — Travail autour du shift add pour créer des divisions.

### 02. Réflexions personnelles
* [`Bresenham`](docs/unfinishedBresenham.md) — Comprendre le fonctionnement de Bresenham par l'hydraulique.

### 03. Projets en cours

---

## 🛠️ Configuration
Ce projet est rédigé en C standard. Le fichier `.gitignore` est configuré pour exclure les binaires et fichiers objets compilés.
