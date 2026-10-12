#include "../include/types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Déclarations externes (à terme, il faudra créer un shape.h pour éviter ça)
extern scene_t *create_scene(int width, int height);
extern void free_scene(scene_t *scene);
extern shape_t *create_rectangle(scene_t *scene, int x, int y, int width,
                                 int height, char *color_stroke,
                                 char *color_fill);
extern void add_shape_to_scene(scene_t *scene, shape_t *shape);
extern void export_to_svg(scene_t *scene, const char *filename);

// Fonction utilitaire temporaire pour debugger la liste chaînée
void print_scene(scene_t *scene) {
  printf("--- ETAT DE LA SCENE ---\n");
  if (scene->head == NULL) {
    printf("La scene est vide.\n");
    return;
  }

  shape_t *curr = scene->head;
  while (curr != NULL) {
    if (curr->type == RECTANGLE) {
      printf("[ID: %d] RECTANGLE - Pos(%d,%d) Dim(%dx%d) Stroke:%s Fill:%s\n",
             curr->id, curr->data.rect.x, curr->data.rect.y,
             curr->data.rect.width, curr->data.rect.height,
             curr->style.color_stroke, curr->style.color_fill);
    }
    curr = curr->next;
  }
  printf("------------------------\n");
}

int main(void) {
  char input[256];

  // 1. Initialisation de la scène au lancement
  scene_t *scene = create_scene(800, 800);
  if (scene == NULL) {
    printf("Erreur critique: impossible d'initialiser la scene.\n");
    return 1;
  }

  printf("=== EDITEUR SVG (Vertical Slice) ===\n");

  while (true) {
    printf("svg> ");
    if (fgets(input, sizeof(input), stdin) == NULL)
      break;
    input[strcspn(input, "\n")] = '\0'; // Retire le saut de ligne[cite: 5]

    if (strcmp(input, "quit") == 0) { //[cite: 5]
      break;
    } else if (strcmp(input, "help") == 0) { //[cite: 5]
      printf("Commandes disponibles :\n");
      printf("  create rect X Y W H STROKE FILL\n");
      printf("  list\n");
      printf("  quit\n");
    } else if (strcmp(input, "list") == 0) {
      // 2. Affichage du contenu de la mémoire
      print_scene(scene);
    } else if (strcmp(input, "display") == 0) {
      export_to_svg(scene, "temp_render.svg");
    } else if (strncmp(input, "create rect", 11) == 0) {
      int x, y, w, h;
      char stroke[10], fill[10];

      // 3. Parsing des arguments tapés par l'utilisateur
      if (sscanf(input, "create rect %d %d %d %d %9s %9s", &x, &y, &w, &h,
                 stroke, fill) == 6) {
        shape_t *rect = create_rectangle(scene, x, y, w, h, stroke, fill);
        if (rect != NULL) {
          add_shape_to_scene(scene, rect);
          printf("> Rectangle cree avec l'ID %d.\n", rect->id);
        }
      } else {
        printf("> Erreur syntaxe. Usage: create rect x y w h stroke fill\n");
      }
    } else if (strcmp(input, "") == 0) { //[cite: 5]
      // On ne fait rien si l'utilisateur appuie juste sur Entrée
    } else {
      printf("unknown command\n"); //[cite: 5]
    }
  }

  // 4. Nettoyage absolu avant la fermeture
  free_scene(scene);
  printf("Fermeture propre de l'editeur.\n");
  return 0;
}
