#define _XOPEN_SOURCE 500

#include "../include/cli.h"
#include "../include/geometry.h"
#include "../include/group.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void get_input(char *buffer, int size) {
  if (fgets(buffer, size, stdin))
    buffer[strcspn(buffer, "\n")] = '\0';
  else
    buffer[0] = '\0';
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

  if (node->type == RECTANGLE) {
    if (node->data.rect.width == node->data.rect.height) {
      printf(C_CYAN "├── 🟥 [%d] Square   " RESET
                    "  | Pos(%d,%d) Side:%d " C_YELLOW "🔍 x%.2f" RESET
                    " | 🖌️ S:%s F:%s\n",
             node->id, node->data.rect.x + mx, node->data.rect.y + my,
             node->data.rect.width, sc, st, fi);
    } else {
      printf(C_CYAN "├── ▭ [%d] Rectangle" RESET
                    "  | Pos(%d,%d) Dim(%dx%d) " C_YELLOW "🔍 x%.2f" RESET
                    " | 🖌️ S:%s F:%s\n",
             node->id, node->data.rect.x + mx, node->data.rect.y + my,
             node->data.rect.width, node->data.rect.height, sc, st, fi);
    }
  } else if (node->type == CIRCLE)
    printf(C_GREEN "├──  ⚪  [%d] Circle   " RESET
                   "  | Center(%d,%d) Rad:%d " C_YELLOW "🔍 x%.2f" RESET
                   " | 🖌️ S:%s F:%s\n",
           node->id, node->data.circle.cx + mx, node->data.circle.cy + my,
           node->data.circle.r, sc, st, fi);
  else if (node->type == ELLIPSE)
    printf(C_MAGENTA "├── ⬭ [%d] Ellipse  " RESET
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
           (node->type == POLYLINE) ? "📈" : "⬡", node->id,
           (node->type == POLYLINE) ? "PolyLine " : "Polygon ",
           node->data.point_list.count, sc, st, fi);
  else if (node->type == GROUP) {
    printf(C_YELLOW BOLD "├── 📁 [%d] GROUP    " RESET C_YELLOW
                         "  | 🔍 Global Scale: x%.2f\n" RESET,
           node->id, sc);
    print_recursive(node->data.group.head, depth + 1);
  }
  print_recursive(node->next, depth);
}

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

void press_enter(void) {
  printf("\n%s  👉 Press [Enter] to continue...%s", DIM, RESET);
  int c;
  while ((c = getchar()) != '\n' && c != EOF) {
  }
}

int ask_int(const char *title, const char *hint, const char *color) {
  char buf[128];
  long long val;
  while (true) {
    printf("\n%s╭─ %s %s\n", color, title, RESET);
    printf("%s│  %s💡 Hint: %s%s\n", color, DIM, hint, RESET);
    printf("%s╰─👉 %s", color, RESET);
    get_input(buf, sizeof(buf));

    if (sscanf(buf, "%lld", &val) == 1) {
      if (val < -100000 || val > 100000) {
        printf(C_RED "    ✖ Value out of bounds. Please enter a reasonable "
                     "coordinate (-100000 to 100000).\n" RESET);
      } else {
        return (int)val;
      }
    } else {
      printf(C_RED
             "    ✖ Input Error: Please enter valid integers only.\n" RESET);
    }
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

void print_options(void) {
  printf(C_GREEN
         "╭── 🛠️  CREATION TOOLS ────────────────────────────────\n" RESET);
  printf(C_GREEN "│" RESET "  [1] ➕ Create a new shape\n");
  printf(C_GREEN "│" RESET "  [2] 📍 Add a point (for Polyline or Polygon)\n");
  printf(C_GREEN
         "╰──────────────────────────────────────────────────────\n" RESET);

  printf(C_YELLOW
         "╭── 📐 EDITING & STRUCTURE ────────────────────────────\n" RESET);
  printf(C_YELLOW "│" RESET "  [3] 📐 Transform shape / Edit colors\n");
  printf(C_YELLOW "│" RESET "  [4] 📁 Manage grouped elements (N-ary tree)\n");
  printf(C_YELLOW "│" RESET "  [5] 🗑️  Delete an element from the scene\n");
  printf(C_YELLOW
         "╰──────────────────────────────────────────────────────\n" RESET);

  printf(C_MAGENTA
         "╭── 💾 FILES & EXPORT ─────────────────────────────────\n" RESET);
  printf(C_MAGENTA "│" RESET "  [6] 💾 Save or Load project (.mysvg)\n");
  printf(C_MAGENTA "│" RESET "  [7] 🖵 Visualization of the SVG\n");
  printf(C_MAGENTA
         "╰──────────────────────────────────────────────────────\n" RESET);

  printf(DIM "     [0] 🚪 Quit application\n" RESET);
}

void print_dashboard(scene_t *scene) {
  system("clear");
  printf(C_BLUE BOLD "═════════════════════════════════════════════════════════"
                     "══════════════════════\n" RESET);
  printf(C_WHITE BOLD "                         ✨ SVG editor ✨ "
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
                 "  [1] ▭ Rectangle      [4] ⬭ Ellipse     [7] ⬡ Polygon\n");
  printf(C_GREEN "│" RESET
                 "  [2] 🟥 Square        [5] ➖ Line       [8] 📁 Group\n");
  printf(C_GREEN
         "│" RESET
         "  [3] ⚪ Circle        [6] 📈 Polyline    [9] 😃 Steamhappy\n");
  printf(
      C_GREEN
      "╰─────────────────────────────────────────────────────────────\n" RESET);

  int type = ask_int("Which shape do you want to create?",
                     "Enter a number between 1 and 9", C_GREEN);
  if (type < 1 || type > 9)
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
  } else if (type == 9) {
    p1 = ask_int("Center X position", "Horizontal center", C_CYAN);
    p2 = ask_int("Center Y position", "Vertical center", C_CYAN);
    p3 = ask_int("Smiley Size", "Reference size in pixels (e.g. 100)", C_CYAN);
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
  else if (type == 9)
    s = create_steamhappy(scene, p1, p2, p3);

  if (s)
    add_shape_to_scene(scene, s);
  press_enter();
}
void page_modify(scene_t *scene) {
  system("clear");
  printf(C_YELLOW BOLD "\n  📐 EDIT MODE\n  ────────────\n\n" RESET);

  if (scene->head == NULL) {
    printf(DIM
           "      ( The scene is empty. No elements to modify. )\n\n" RESET);
    press_enter();
    return;
  } else {
    printf(C_CYAN "  Current Scene Elements:\n" RESET);
    print_recursive(scene->head, 0);
    printf("\n");
  }

  int id = ask_int("Target element ID to modify",
                   "Check the bracketed number [] above. Type 0 to cancel.",
                   C_YELLOW);
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
  if (act < 1 || act > 6) {
    printf(C_RED "\n  ✖ Invalid action choice.\n" RESET);
    press_enter();
    return;
  }

  if (act == 1) {
    s->transform.move_x += ask_int("Horizontal offset (X)",
                                   "Positive = right, Negative = left", C_CYAN);
    s->transform.move_y += ask_int("Vertical offset (Y)",
                                   "Positive = down, Negative = up", C_CYAN);
  } else if (act == 2) {
    s->transform.angle =
        (s->transform.angle +
         ask_int("Rotation angle", "In degrees (e.g., 45, 90, 180)", C_CYAN)) %
        360;
  } else if (act == 3) {
    int pct = ask_int("Scale Factor",
                      "Type 100 for 100%, 200 to double, 50 for half", C_CYAN);
    if (pct <= 0) {
      printf(C_RED "\n  ✖ Scale must be greater than 0.\n" RESET);
    } else {
      s->transform.scale_factor = pct / 100.0f;
    }
  } else if (act == 4) {
    s->transform.invert_x = !s->transform.invert_x;
  } else if (act == 5) {
    s->transform.invert_y = !s->transform.invert_y;
  } else if (act == 6) {
    ask_string("New stroke color", "e.g., black, none", s->style.color_stroke,
               32, C_MAGENTA);
    ask_string("New fill color", "e.g., red, none", s->style.color_fill, 32,
               C_MAGENTA);
  }

  animate_loading("Applying mathematical transformations", C_YELLOW);
  press_enter();
}
