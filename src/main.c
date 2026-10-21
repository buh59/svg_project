#include "../include/cli.h"
#include "../include/export.h"
#include "../include/geometry.h"
#include "../include/group.h"
#include "../include/scene.h"
#include "../include/storage.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

bool valid_name(const char *s) {
  if (*s == '\0')
    return false;
  for (; *s; s++)
    if (!isalnum((unsigned char)*s) && *s != '_' && *s != '-')
      return false;
  return true;
}

int main(void) {
  scene_t *scene = create_scene(800, 800);
  if (scene == NULL) {
    fprintf(stderr, "Error: out of memory.\n");
    return 1;
  }

  while (true) {
    print_dashboard(scene);
    print_options();
    int choice =
        ask_int("What would you like to do?",
                "Enter the corresponding action number (0-7)", C_WHITE);

    if (choice == 0)
      break;
    else if (choice == 1)
      page_create(scene);
    else if (choice == 3)
      page_modify(scene);

    else if (choice == 2) {
      int id = ask_int("Polyline or Polygon ID",
                       "Look for the ID bracketed in the tree view", C_GREEN);
      shape_t *s = find_shape_by_id(scene->head, id);
      if (s && (s->type == POLYLINE || s->type == POLYGON)) {
        int px = ask_int("X Coordinate", "Horizontal position", C_CYAN);
        int py = ask_int("Y Coordinate", "Vertical position", C_CYAN);
        animate_loading("Appending point to the list", C_GREEN);
        add_point(s, px, py);
      } else {
        printf(
            C_RED
            "  ✖ Error: Invalid ID or not a valid polygon/polyline.\n" RESET);
      }
      press_enter();
    } else if (choice == 4) {
      int act = ask_int("Group Action", "1 to Insert, 2 to Extract", C_YELLOW);
      if (act == 1) {
        int target_id = ask_int("ID to move", "Element to insert", C_CYAN);
        int group_id =
            ask_int("Target Group ID", "Must be a Folder 📁", C_CYAN);
        shape_t *s = find_shape_by_id(scene->head, target_id);
        shape_t *g = find_shape_by_id(scene->head, group_id);

        if (s && g && g->type == GROUP && s != g && !is_ancestor(s, g)) {
          animate_loading("Restructuring N-ary tree relationships", C_YELLOW);
          detach_shape(s, scene);
          add_shape_to_group(g, s);
        } else {
          printf(C_RED "  ✖ Action failed (Risk of infinite loop or invalid "
                       "IDs).\n" RESET);
        }
      } else if (act == 2) {
        int id_to_extract = ask_int(
            "ID to extract", "Element will be placed back to the root", C_CYAN);
        shape_t *s = find_shape_by_id(scene->head, id_to_extract);

        if (s && s->parent != NULL) {
          animate_loading("Extracting element from group", C_YELLOW);
          remove_from_group(scene, id_to_extract);
        } else {
          printf(C_RED "  ✖ Error: ID [%d] is invalid or not currently inside "
                       "a group.\n" RESET,
                 id_to_extract);
        }
      }
      press_enter();
    } else if (choice == 5) {
      int id_to_delete = ask_int("Target element ID to delete",
                                 "⚠️ Warning, irreversible action", C_RED);
      shape_t *s = find_shape_by_id(scene->head, id_to_delete);

      if (s) {
        animate_loading("Performing clean memory deletion", C_RED);
        delete_shape(scene, id_to_delete);
      } else {
        printf(C_RED "  ✖ Error: ID [%d] not found in the scene.\n" RESET,
               id_to_delete);
      }
      press_enter();
    } else if (choice == 6) {
      int sl = ask_int("File Action", "1 to Save (mysvg + svg), 2 to Load",
                       C_MAGENTA);

      if (sl == 1) {
        char base_name[100];
        ask_string("Project Name",
                   "Letters, digits, _ and - only (e.g., my_drawing)",
                   base_name, 100, C_MAGENTA);

        if (!valid_name(base_name)) {
          printf(C_RED "  ✖ Error: invalid name. Use only letters, digits, "
                       "'_' and '-'.\n" RESET);
        } else {
          char mysvg_file[128], svg_file[128];
          snprintf(mysvg_file, sizeof(mysvg_file), "%s.mysvg", base_name);
          snprintf(svg_file, sizeof(svg_file), "%s.svg", base_name);

          animate_loading("Generating dual backup", C_MAGENTA);
          save_project(scene, mysvg_file);
          export_to_svg(scene, svg_file);
        }

      } else if (sl == 2) {
        char file[128];
        ask_string("File to load",
                   "Must include .mysvg (e.g., my_drawing.mysvg)", file, 128,
                   C_MAGENTA);

        FILE *f = fopen(file, "r");
        if (f) {
          fclose(f);
          animate_loading("Accessing file system", C_MAGENTA);
          scene_t *loaded = load_project(file);
          if (loaded) {
            free_scene(scene);
            scene = loaded;
          }
        } else {
          printf(C_RED "  ✖ Error: Could not find or open file '%s'.\n" RESET,
                 file);
        }
      }
      press_enter();
    } else if (choice == 7) {
      animate_loading("Compiling and generating vector output", C_BLUE);
      export_to_svg(scene, "render.svg");
      open_in_viewer("render.svg");
      press_enter();
    }
  }

  free_scene(scene);
  printf(CLEAR);
  printf(C_GREEN BOLD "\n  ✨ Thank you for using this SVG editor.\n" RESET);
  return 0;
}
