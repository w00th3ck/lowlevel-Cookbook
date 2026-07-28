# Bitwise & Low-Level Engineering Cookbook

> *"Who am I, and what do my algorithms say about me?"*

 Ce recueil documente mes recherches, mes fiches techniques et l'évolution de ma pratique du C bas niveau. L'objectif de cette démarche est de privilégier la compréhension du silicium, le zéro-branchement (*branchless*) et la manipulation directe des registres.

---

##  La Philosophie
- **Algèbre de Boole & Silicium :** Traiter le code non comme une abstraction, mais comme une équation booléenne exécutée par l'ALU.
- **Zero-Branching & Déterminisme :** Éliminer les conditions (`if`, `while`) au profit d'opérateurs logiques (`&`, `|`, `^`, `~`, `<<`, `>>`) pour garantir un temps d'exécution constant.
- **Émergence par la contrainte :** Refuser de consommer des solutions toutes faites pour laisser les propriétés mathématiques émerger de la structure des bits.

 [Lire la Genèse complète du projet dans docs/GENESIS.md](docs/GENESIS.md)

---

##  Index des Recettes

### 01. Fondations Logiques
* [`01_nand_to_alu.md`](recipes/01_nand_to_alu.md) — Reconstruire la boîte à outils binaire à partir de la porte universelle NAND.
* [`02_branchless_add.md`](recipes/02_branchless_add.md) — De la boucle conditionnelle à l'additionneur binaire déterministe.

### 02. Systèmes Témoins & Registres
* [`03_tic_tac_toe_reg.md`](recipes/03_tic_tac_toe_reg.md) — Faire tenir un jeu de Morpion complet et son IA dans un unique registre 32 bits.

---

## 🛠️ Configuration
Ce projet est rédigé en C standard. Le fichier `.gitignore` est configuré pour exclure les binaires et fichiers objets compilés.
