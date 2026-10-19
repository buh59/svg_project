#define _XOPEN_SOURCE 500

#include "../include/export.h"
#include "../include/geometry.h"
#include "../include/group.h"
#include "../include/scene.h"
#include "../include/storage.h"
#include "../include/types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CLEAR "\033[H\033[J"
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define RESET "\033[0m"

#define C_CYAN "\033[96m"
#define C_GREEN "\033[92m"
#define C_YELLOW "\033[93m"
#define C_MAGENTA "\033[95m"
#define C_BLUE "\033[94m"
#define C_RED "\033[91m"
#define C_WHITE "\033[97m"

void animate_loading(const char *message, const char *color) {
  printf("\n%s╭── ⏳ %s ────────────────────────%s\n", color, message, RESET);
  printf("%s│%s [", color, C_WHITE);
  for (int i = 0; i < 20; i++) {
    printf("%s█%s", C_GREEN, RESET);
    fflush(stdout);
    usleep(25000);
  }
  printf("%s] %s✔ Done%s\n", C_WHITE, C_GREEN, RESET);
  printf("%s╰─────────────────────────────────────────%s\n", color, RESET);
  usleep(400000);
}

void get_input(char *buffer, int size) {
  if (fgets(buffer, size, stdin))
    buffer[strcspn(buffer, "\n")] = '\0';
  else
    buffer[0] = '\0';
}

void press_enter() {
  printf("\n%s  👉 Press [Enter] to continue...%s", DIM, RESET);
  char buf[2];
  get_input(buf, sizeof(buf));
}

int ask_int(const char *title, const char *hint, const char *color) {
  char buf[128];
  int val;
  while (true) {
    printf("\n%s╭─ %s %s\n", color, title, RESET);
    printf("%s│  %s💡 Hint: %s%s\n", color, DIM, hint, RESET);
    printf("%s╰─👉 %s", color, RESET);
    get_input(buf, sizeof(buf));

    if (sscanf(buf, "%d", &val) == 1)
      return val;

    printf(C_RED
           "    ✖ Input Error: Please enter valid integers only.\n" RESET);
  }
}

void ask_string(const char *title, const char *hint, char *buffer, int size,
                const char *color) {
  while (true) {
    printf("\n%s╭─ %s %s\n", color, title, RESET);
    printf("%s│  %s💡 Hint: %s%s\n", color, DIM, hint, RESET);
    printf("%s╰─👉 %s", color, RESET);
    get_input(buffer, size);

    if (strlen(buffer) > 0)
      return;

    printf(C_RED "    ✖ Error: This field cannot be left empty.\n" RESET);
  }
}

void print_recursive(shape_t *node, int depth) {
  if (!node)
    return;
  printf("    ");
  for (int i = 0; i < depth; i++)
    printf(C_CYAN "│   " RESET);

  int mx = node->transform.move_x, my = node->transform.move_y;
  float sc = node->transform.scale_factor;
  char st[32], fi[32];
  strcpy(st, node->style.color_stroke[0] ? node->style.color_stroke : "none");
  strcpy(fi, node->style.color_fill[0] ? node->style.color_fill : "none");

  if (node->type == RECTANGLE)
    printf(C_CYAN "├── 🟦 [%d] Rectangle" RESET
                  "  | Pos(%d,%d) Dim(%dx%d) " C_YELLOW "🔍 x%.2f" RESET
                  " | 🖌️ S:%s F:%s\n",
           node->id, node->data.rect.x + mx, node->data.rect.y + my,
           node->data.rect.width, node->data.rect.height, sc, st, fi);
  else if (node->type == CIRCLE)
    printf(C_GREEN "├── 🟢 [%d] Circle   " RESET
                   "  | Center(%d,%d) Rad:%d " C_YELLOW "🔍 x%.2f" RESET
                   " | 🖌️ S:%s F:%s\n",
           node->id, node->data.circle.cx + mx, node->data.circle.cy + my,
           node->data.circle.r, sc, st, fi);
  else if (node->type == ELLIPSE)
    printf(C_MAGENTA "├── 🟣 [%d] Ellipse  " RESET
                     "  | Center(%d,%d) Rad(%d,%d) " C_YELLOW "🔍 x%.2f" RESET
                     " | 🖌️ S:%s F:%s\n",
           node->id, node->data.ellipse.cx + mx, node->data.ellipse.cy + my,
           node->data.ellipse.rx, node->data.ellipse.ry, sc, st, fi);
  else if (node->type == LINE)
    printf(C_WHITE "├── ➖ [%d] Line     " RESET
                   "  | P1(%d,%d) P2(%d,%d) " C_YELLOW "🔍 x%.2f" RESET
                   " | 🖌️ S:%s\n",
           node->id, node->data.line.p1.x + mx, node->data.line.p1.y + my,
           node->data.line.p2.x + mx, node->data.line.p2.y + my, sc, st);
  else if (node->type == POLYLINE || node->type == POLYGON)
    printf(C_BLUE "├── %s [%d] %s" RESET "  | %zu pts " C_YELLOW
                  "🔍 x%.2f" RESET " | 🖌️ S:%s F:%s\n",
           (node->type == POLYLINE) ? "📈" : "🔺", node->id,
           (node->type == POLYLINE) ? "PolyLgn " : "Polygon ",
           node->data.point_list.count, sc, st, fi);
  else if (node->type == GROUP) {
    printf(C_YELLOW BOLD "├── 📁 [%d] GROUP    " RESET C_YELLOW
                         "  | 🔍 Global Scale: x%.2f\n" RESET,
           node->id, sc);
    print_recursive(node->data.group.head, depth + 1);
  }
  print_recursive(node->next, depth);
}

void print_dashboard(scene_t *scene) {
  system("clear");
  printf(C_BLUE BOLD "═════════════════════════════════════════════════════════"
                     "══════════════════════\n" RESET);
  printf(C_WHITE BOLD "                         ✨ VECTORIS SVG STUDIO v5.0 ✨ "
                      "                       \n" RESET);
  printf(C_BLUE BOLD "═════════════════════════════════════════════════════════"
                     "══════════════════════\n\n" RESET);

  printf(C_CYAN "╭── 📊 CURRENT SCENE (Viewport: %dx%d) "
                "───────────────────────────────\n" RESET,
         scene->view_width, scene->view_height);
  printf(C_CYAN "│\n" RESET);
  if (scene->head == NULL)
    printf(C_CYAN
           "│" RESET DIM
           "      ( The scene is empty. Start by creating a shape! )\n" RESET);
  else
    print_recursive(scene->head, 0);
  printf(C_CYAN "│\n╰──────────────────────────────────────────────────────────"
                "───────────────────\n" RESET);
}

void page_create(scene_t *scene) {
  system("clear");
  printf(C_GREEN BOLD
         "\n  🎨 SHAPE CREATION MODE\n  ──────────────────────\n\n" RESET);

  printf(
      C_GREEN
      "╭── SELECTION ────────────────────────────────────────────────\n" RESET);
  printf(C_GREEN "│" RESET
                 "  [1] 🟦 Rectangle    [3] 🟢 Circle      [5] ➖ Line\n");
  printf(C_GREEN "│" RESET
                 "  [2] 🟩 Square       [4] 🟣 Ellipse     [6] 📈 Polyline\n");
  printf(C_GREEN "│" RESET "  [8] 📁 Group        [7] 🔺 Polygon\n");
  printf(
      C_GREEN
      "╰─────────────────────────────────────────────────────────────\n" RESET);

  int type = ask_int("Which shape do you want to create?",
                     "Enter a number between 1 and 8", C_GREEN);
  if (type < 1 || type > 8)
    return;

  int p1 = 0, p2 = 0, p3 = 0, p4 = 0;
  char stroke[32] = "black", fill[32] = "none";

  printf(C_CYAN "\n  📍 COORDINATES SETUP\n  ────────────────────\n" RESET);

  if (type == 1) {
    p1 = ask_int("Starting X position", "Horizontal coordinate in pixels",
                 C_CYAN);
    p2 =
        ask_int("Starting Y position", "Vertical coordinate in pixels", C_CYAN);
    p3 = ask_int("Rectangle width", "Horizontal size", C_CYAN);
    p4 = ask_int("Rectangle height", "Vertical size", C_CYAN);
  } else if (type == 2) {
    p1 = ask_int("Starting X position", "Horizontal coordinate in pixels",
                 C_CYAN);
    p2 =
        ask_int("Starting Y position", "Vertical coordinate in pixels", C_CYAN);
    p3 = ask_int("Side size", "A single number for width and height", C_CYAN);
    p4 = p3;
  } else if (type == 3) {
    p1 =
        ask_int("Center X of the circle", "Horizontal center position", C_CYAN);
    p2 = ask_int("Center Y of the circle", "Vertical center position", C_CYAN);
    p3 = ask_int("Circle radius", "Radius size in pixels", C_CYAN);
  } else if (type == 4) {
    p1 = ask_int("Center X of the ellipse", "Horizontal center position",
                 C_CYAN);
    p2 = ask_int("Center Y of the ellipse", "Vertical center position", C_CYAN);
    p3 = ask_int("Horizontal Radius (RX)", "Span on the X axis", C_CYAN);
    p4 = ask_int("Vertical Radius (RY)", "Span on the Y axis", C_CYAN);
  } else if (type == 5) {
    p1 = ask_int("Starting Point (X1)", "X coordinate of the first point",
                 C_CYAN);
    p2 = ask_int("Starting Point (Y1)", "Y coordinate of the first point",
                 C_CYAN);
    p3 = ask_int("Ending Point (X2)", "X coordinate of the second point",
                 C_CYAN);
    p4 = ask_int("Ending Point (Y2)", "Y coordinate of the second point",
                 C_CYAN);
  }

  if (type >= 1 && type <= 7) {
    printf(C_MAGENTA "\n  🎨 COLORS SETUP\n  ───────────────\n" RESET);
    ask_string("Stroke color", "English keyword (e.g., black, red) or HEX code",
               stroke, 32, C_MAGENTA);
    if (type != 5 && type != 6) {
      ask_string("Fill color", "Use 'none' for transparent", fill, 32,
                 C_MAGENTA);
    }
  }

  animate_loading("Generating and allocating shape data", C_GREEN);

  shape_t *s = NULL;
  if (type == 1 || type == 2)
    s = create_rectangle(scene, p1, p2, p3, p4, stroke, fill);
  else if (type == 3)
    s = create_circle(scene, p1, p2, p3, stroke, fill);
  else if (type == 4)
    s = create_ellipse(scene, p1, p2, p3, p4, stroke, fill);
  else if (type == 5)
    s = create_line(scene, p1, p2, p3, p4, stroke, "none");
  else if (type == 6)
    s = create_point_list_shape(scene, POLYLINE, stroke, "none");
  else if (type == 7)
    s = create_point_list_shape(scene, POLYGON, stroke, fill);
  else if (type == 8)
    s = create_group(scene);

  if (s)
    add_shape_to_scene(scene, s);
  press_enter();
}

void page_modify(scene_t *scene) {
  system("clear");
  printf(C_YELLOW BOLD "\n  📐 EDIT MODE\n  ────────────\n\n" RESET);

  int id = ask_int(
      "Target element ID to modify",
      "Check the bracketed number [] in the tree. Type 0 to cancel.", C_YELLOW);
  if (id == 0)
    return;

  shape_t *s = find_shape_by_id(scene->head, id);
  if (!s) {
    printf(C_RED "\n  ✖ Cannot find ID [%d] in the scene.\n" RESET, id);
    press_enter();
    return;
  }

  printf(C_YELLOW "\n╭── TRANSFORMATION CHOICE "
                  "────────────────────────────────────\n" RESET);
  printf(C_YELLOW
         "│" RESET
         "  [1] 🚚 Move                   [4] ↔️  Horizontal Mirror (Flip X)\n");
  printf(C_YELLOW
         "│" RESET
         "  [2] 🔄 Rotate                 [5] ↕️  Vertical Mirror (Flip Y)\n");
  printf(C_YELLOW "│" RESET
                  "  [3] 🔍 Scale (Change size)    [6] 🎨 Change Colors\n");
  printf(
      C_YELLOW
      "╰─────────────────────────────────────────────────────────────\n" RESET);

  int act =
      ask_int("Action to apply", "Enter a number between 1 and 6", C_YELLOW);

  if (act == 1) {
    s->transform.move_x += ask_int("Horizontal offset (X)",
                                   "Positive = right, Negative = left", C_CYAN);
    s->transform.move_y += ask_int("Vertical offset (Y)",
                                   "Positive = down, Negative = up", C_CYAN);
  } else if (act == 2)
    s->transform.angle =
        (s->transform.angle +
         ask_int("Rotation angle", "In degrees (e.g., 45, 90, 180)", C_CYAN)) %
        360;
  else if (act == 3)
    s->transform.scale_factor =
        ask_int("Scale Factor", "Type 100 for 100%, 200 to double, 50 for half",
                C_CYAN) /
        100.0f;
  else if (act == 4)
    s->transform.invert_x = !s->transform.invert_x;
  else if (act == 5)
    s->transform.invert_y = !s->transform.invert_y;
  else if (act == 6) {
    ask_string("New stroke color", "e.g., black, none", s->style.color_stroke,
               32, C_MAGENTA);
    ask_string("New fill color", "e.g., red, none", s->style.color_fill, 32,
               C_MAGENTA);
  }

  animate_loading("Applying mathematical transformations", C_YELLOW);
  press_enter();
}

int main(void) {
  scene_t *scene = create_scene(800, 800);

  while (true) {
    print_dashboard(scene);

    printf(C_GREEN
           "╭── 🛠️  CREATION TOOLS ────────────────────────────────\n" RESET);
    printf(C_GREEN "│" RESET "  [1] ➕ Create a new shape\n");
    printf(C_GREEN "│" RESET
                   "  [2] 📍 Add a point (for Polyline or Polygon)\n");
    printf(C_GREEN
           "╰──────────────────────────────────────────────────────\n" RESET);

    printf(C_YELLOW
           "╭── 📐 EDITING & STRUCTURE ────────────────────────────\n" RESET);
    printf(C_YELLOW "│" RESET "  [3] 📐 Transform shape / Edit colors\n");
    printf(C_YELLOW "│" RESET
                    "  [4] 📁 Manage grouped elements (N-ary tree)\n");
    printf(C_YELLOW "│" RESET "  [5] 🗑️  Delete an element from the scene\n");
    printf(C_YELLOW
           "╰──────────────────────────────────────────────────────\n" RESET);

    printf(C_MAGENTA
           "╭── 💾 FILES & EXPORT ─────────────────────────────────\n" RESET);
    printf(C_MAGENTA "│" RESET "  [6] 💾 Save or Load project (.mysvg)\n");
    printf(C_MAGENTA "│" RESET "  [7] 🚀 Export final SVG render\n");
    printf(C_MAGENTA
           "╰──────────────────────────────────────────────────────\n" RESET);

    printf(DIM "     [0] 🚪 Quit application\n" RESET);

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
        animate_loading("Appending point to the list", C_GREEN);
        add_point(s, ask_int("X Coordinate", "Horizontal position", C_CYAN),
                  ask_int("Y Coordinate", "Vertical position", C_CYAN));
      } else {

        printf(
            C_RED
            "\n  ✖ Error: ID [%d] is invalid or not a valid polygon.\n" RESET,
            id);
      }
      press_enter();
    }

    else if (choice == 4) {
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
          printf(C_RED "\n  ✖ Action failed (Risk of infinite loop, or invalid "
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
          printf(C_RED "\n  ✖ Error: ID [%d] is invalid or not currently "
                       "inside a group.\n" RESET,
                 id_to_extract);
        }
      }
      press_enter();
    }

    else if (choice == 5) {
      int id_to_delete = ask_int("Target element ID to delete",
                                 "⚠️ Warning, irreversible action", C_RED);
      shape_t *s = find_shape_by_id(scene->head, id_to_delete);

      if (s) {
        animate_loading("Performing clean memory deletion", C_RED);
        delete_shape(scene, id_to_delete);
      } else {
        printf(C_RED "\n  ✖ Error: ID [%d] not found in the scene.\n" RESET,
               id_to_delete);
      }
      press_enter();
    }

    else if (choice == 6) {
      int sl = ask_int("File Action", "1 to Save (mysvg + svg), 2 to Load",
                       C_MAGENTA);

      if (sl == 1) {
        char base_name[100];
        ask_string("Project Name",
                   "Enter the name WITHOUT extension (e.g., my_drawing)",
                   base_name, 100, C_MAGENTA);

        char mysvg_file[128], svg_file[128];
        snprintf(mysvg_file, sizeof(mysvg_file), "%s.mysvg", base_name);
        snprintf(svg_file, sizeof(svg_file), "%s.svg", base_name);

        animate_loading("Generating dual backup", C_MAGENTA);
        save_project(scene, mysvg_file);
        export_to_svg(scene, svg_file);

        printf(C_GREEN "  ✅ Project saved to '%s'\n" RESET, mysvg_file);
        printf(C_GREEN "  ✅ Render exported to '%s'\n" RESET, svg_file);

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
          printf(C_RED "\n  ✖ Error: Could not find or open file '%s'.\n" RESET,
                 file);
        }
      }
      press_enter();
    } else if (choice == 7) {
      animate_loading("Compiling and generating vector output", C_BLUE);
      export_to_svg(scene, "render.svg");
      press_enter();
    }
  }

  free_scene(scene);
  system("clear");
  printf(C_GREEN BOLD "\n  ✨ Thank you for using Vectoris SVG Studio. Memory "
                      "cleanly freed!\n\n" RESET);
  return 0;
}
