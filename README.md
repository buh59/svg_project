# SVG Editor

**SVG Studio** est un éditeur graphique vectoriel interactif en ligne de commande (TUI), développé entièrement en **C99 strict**. Il permet de créer, manipuler, organiser en groupes et exporter des formes géométriques au format standard SVG.

L'interface utilisateur a été pensée pour offrir une expérience ergonomique ("Focus Mode"), remplaçant les longues commandes textuelles par des menus interactifs, sécurisés et visuellement agréables, sans nécessiter la moindre librairie externe.

---

## ✨ Fonctionnalités Principales

- **🧰 Formes supportées :** Rectangles, Carrés, Cercles, Ellipses, Lignes, Polylignes et Polygones.
- **📐 Transformations Mathématiques :** Déplacement (Translation), Rotation, Mise à l'échelle (Scale factor), et Inversions axiales (Flip X / Flip Y).
- **📁 Arbre N-aire (Groupes) :** Regroupement de formes à l'infini. Les transformations appliquées à un groupe se répercutent mathématiquement sur tous ses enfants.
- **💾 Double Sauvegarde Automatique :**
  - Un fichier de travail interne (`.mysvg`) pour conserver l'arborescence.
  - Un export standard (`.svg`) lisible par n'importe quel navigateur web.
- **🛡️ Saisies Sécurisées :** L'interface ne crashe jamais grâce à un buffer de lecture personnalisé (`fgets`) qui remplace les appels `scanf` instables.

---

## 🏗️ Architecture et Choix Techniques

Ce projet repose sur une séparation stricte des responsabilités (fichiers `.h` et `.c` modulaires) et sur des structures de données avancées :

1. **Graphe de Scène (Scene Graph) :**
   Les éléments sont stockés sous la forme d'un **arbre N-aire** couplé à des **listes doublement chaînées**. Cela permet un parcours récursif fluide lors du rendu SVG et une gestion propre des profondeurs de groupes.
   Une sécurité anti-boucle infinie (`is_ancestor`) empêche un groupe de s'ajouter à lui-même.

2. **Système de Sauvegarde (Grammaire Algébrique) :**
   Plutôt que d'intégrer un analyseur XML lourd et risqué pour la mémoire, le projet utilise un langage de sauvegarde personnalisé (`.mysvg`). Lors du chargement, le fichier est parsé via un **Automate à Pile** (Pushdown Automaton), ce qui permet de reconstituer parfaitement l'arbre N-aire et les imbrications de groupes.

3. **Gestion de la Mémoire :**
   L'ensemble du cycle de vie des formes (allocation, détachement, suppression récursive) a été rigoureusement testé. Le projet ne présente **aucune fuite mémoire** (Vérifié avec `Valgrind`).

---

## ⚙️ Compilation et Exécution

Ce projet utilise le compilateur **GCC** et respecte la norme **C99**. Aucune librairie externe n'a été utilisée.

### Commandes Makefile

Conformément aux directives, le projet se gère via les commandes suivantes :

```bash
# Compiler le projet et générer l'exécutable
make build

# Nettoyer les fichiers objets et l'exécutable
make clean
```
