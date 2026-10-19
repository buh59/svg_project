#include "../include/storage.h"
#include "../include/geometry.h"
#include "../include/group.h"
#include "../include/scene.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void save_shape_recursive(shape_t *shape, FILE *f, int depth) {
  if (shape == NULL)
    return;

  if (shape->type == GROUP) {
    fprintf(f, "GROUP %d %d %d %d %d %d %f\n", shape->id,
            shape->transform.move_x, shape->transform.move_y,
            shape->transform.angle, shape->transform.invert_x ? 1 : 0,
            shape->transform.invert_y ? 1 : 0, shape->transform.scale_factor);
    shape_t *child = shape->data.group.head;
    while (child != NULL) {
      save_shape_recursive(child, f, depth + 1);
      child = child->next;
    }
    fprintf(f, "END_GROUP\n");
  } else if (shape->type == RECTANGLE) {
    char stroke[32] = "none", fill[32] = "none";
    if (shape->style.color_stroke[0] != '\0')
      strcpy(stroke, shape->style.color_stroke);
    if (shape->style.color_fill[0] != '\0')
      strcpy(fill, shape->style.color_fill);
    fprintf(f, "RECT %d %d %d %d %s %s %d %d %d %d %d %f\n", shape->data.rect.x,
            shape->data.rect.y, shape->data.rect.width, shape->data.rect.height,
            stroke, fill, shape->transform.move_x, shape->transform.move_y,
            shape->transform.angle, shape->transform.invert_x ? 1 : 0,
            shape->transform.invert_y ? 1 : 0, shape->transform.scale_factor);
  } else if (shape->type == CIRCLE) {
    char stroke[32] = "none", fill[32] = "none";
    if (shape->style.color_stroke[0] != '\0')
      strcpy(stroke, shape->style.color_stroke);
    if (shape->style.color_fill[0] != '\0')
      strcpy(fill, shape->style.color_fill);
    fprintf(f, "CIRCLE %d %d %d %s %s %d %d %d %d %d %f\n",
            shape->data.circle.cx, shape->data.circle.cy, shape->data.circle.r,
            stroke, fill, shape->transform.move_x, shape->transform.move_y,
            shape->transform.angle, shape->transform.invert_x ? 1 : 0,
            shape->transform.invert_y ? 1 : 0, shape->transform.scale_factor);
  } else if (shape->type == ELLIPSE) {
    char stroke[32] = "none", fill[32] = "none";
    if (shape->style.color_stroke[0] != '\0')
      strcpy(stroke, shape->style.color_stroke);
    if (shape->style.color_fill[0] != '\0')
      strcpy(fill, shape->style.color_fill);
    fprintf(f, "ELLIPSE %d %d %d %d %s %s %d %d %d %d %d %f\n",
            shape->data.ellipse.cx, shape->data.ellipse.cy,
            shape->data.ellipse.rx, shape->data.ellipse.ry, stroke, fill,
            shape->transform.move_x, shape->transform.move_y,
            shape->transform.angle, shape->transform.invert_x ? 1 : 0,
            shape->transform.invert_y ? 1 : 0, shape->transform.scale_factor);
  } else if (shape->type == LINE) {
    char stroke[32] = "none", fill[32] = "none";
    if (shape->style.color_stroke[0] != '\0')
      strcpy(stroke, shape->style.color_stroke);
    if (shape->style.color_fill[0] != '\0')
      strcpy(fill, shape->style.color_fill);
    fprintf(f, "LINE %d %d %d %d %s %s %d %d %d %d %d %f\n",
            shape->data.line.p1.x, shape->data.line.p1.y, shape->data.line.p2.x,
            shape->data.line.p2.y, stroke, fill, shape->transform.move_x,
            shape->transform.move_y, shape->transform.angle,
            shape->transform.invert_x ? 1 : 0,
            shape->transform.invert_y ? 1 : 0, shape->transform.scale_factor);
  } else if (shape->type == POLYLINE || shape->type == POLYGON) {
    char stroke[32] = "none", fill[32] = "none";
    if (shape->style.color_stroke[0] != '\0')
      strcpy(stroke, shape->style.color_stroke);
    if (shape->style.color_fill[0] != '\0')
      strcpy(fill, shape->style.color_fill);

    const char *tag = (shape->type == POLYLINE) ? "POLYLINE" : "POLYGON";
    fprintf(f, "%s %s %s %d %d %d %d %d %zu %f", tag, stroke, fill,
            shape->transform.move_x, shape->transform.move_y,
            shape->transform.angle, shape->transform.invert_x ? 1 : 0,
            shape->transform.invert_y ? 1 : 0, shape->data.point_list.count,
            shape->transform.scale_factor);

    point_node_t *pt = shape->data.point_list.head;
    while (pt != NULL) {
      fprintf(f, " %d,%d", pt->p.x, pt->p.y);
      pt = pt->next;
    }
    fprintf(f, "\n");
  }
}

void save_project(scene_t *scene, const char *filename) {
  if (scene == NULL)
    return;
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Error: unable to open %s for saving.\n", filename);
    return;
  }
  fprintf(f, "SCENE %d %d %d\n", scene->view_width, scene->view_height,
          scene->next_id);

  shape_t *curr = scene->head;
  while (curr != NULL) {
    save_shape_recursive(curr, f, 0);
    curr = curr->next;
  }

  fclose(f);
  printf("> Project saved successfully in '%s'.\n", filename);
}

static void apply_transform(shape_t *shape, int mx, int my, int angle, int ix,
                            int iy) {
  shape->transform.move_x = mx;
  shape->transform.move_y = my;
  shape->transform.angle = angle;
  shape->transform.invert_x = (ix != 0);
  shape->transform.invert_y = (iy != 0);
}

scene_t *load_project(const char *filename) {
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    printf("Error: unable to read %s.\n", filename);
    return NULL;
  }

  char line[512];
  scene_t *new_scene = NULL;

  shape_t *group_stack[50];
  int stack_depth = 0;

  while (fgets(line, sizeof(line), f)) {
    line[strcspn(line, "\n")] = '\0';

    if (strncmp(line, "SCENE", 5) == 0) {
      int w, h, next_id;
      if (sscanf(line, "SCENE %d %d %d", &w, &h, &next_id) == 3) {
        new_scene = create_scene(w, h);
        if (new_scene)
          new_scene->next_id = next_id;
      }
    } else if (strncmp(line, "GROUP", 5) == 0 && new_scene != NULL) {
      int id, mx, my, angle, ix, iy;
      float scale = 1.0f;
      if (sscanf(line, "GROUP %d %d %d %d %d %d %f", &id, &mx, &my, &angle, &ix,
                 &iy, &scale) == 7) {
        shape_t *group = create_group(new_scene);
        if (group) {
          group->id = id;
          apply_transform(group, mx, my, angle, ix, iy);

          if (stack_depth > 0) {
            add_shape_to_group(group_stack[stack_depth - 1], group);
          } else {
            add_shape_to_scene(new_scene, group);
          }
          group_stack[stack_depth++] = group;
        }
      }
    } else if (strncmp(line, "END_GROUP", 9) == 0) {
      if (stack_depth > 0)
        stack_depth--;
    } else if (strncmp(line, "RECT", 4) == 0 && new_scene != NULL) {
      int x, y, w, h, mx, my, angle, ix, iy;
      float scale = 1.0f;
      char stroke[20], fill[20];
      if (sscanf(line, "RECT %d %d %d %d %19s %19s %d %d %d %d %d %f", &x, &y,
                 &w, &h, stroke, fill, &mx, &my, &angle, &ix, &iy,
                 &scale) == 12) {
        if (strcmp(stroke, "none") == 0)
          stroke[0] = '\0';
        if (strcmp(fill, "none") == 0)
          fill[0] = '\0';
        shape_t *rect = create_rectangle(new_scene, x, y, w, h, stroke, fill);
        if (rect) {
          apply_transform(rect, mx, my, angle, ix, iy);
          if (stack_depth > 0)
            add_shape_to_group(group_stack[stack_depth - 1], rect);
          else
            add_shape_to_scene(new_scene, rect);
        }
      }
    } else if (strncmp(line, "CIRCLE", 6) == 0 && new_scene != NULL) {
      int cx, cy, r, mx, my, angle, ix, iy;
      float scale = 1.0f;
      char stroke[20], fill[20];
      if (sscanf(line, "CIRCLE %d %d %d %19s %19s %d %d %d %d %d %f", &cx, &cy,
                 &r, stroke, fill, &mx, &my, &angle, &ix, &iy, &scale) == 11) {
        if (strcmp(stroke, "none") == 0)
          stroke[0] = '\0';
        if (strcmp(fill, "none") == 0)
          fill[0] = '\0';
        shape_t *circ = create_circle(new_scene, cx, cy, r, stroke, fill);
        if (circ) {
          apply_transform(circ, mx, my, angle, ix, iy);
          if (stack_depth > 0)
            add_shape_to_group(group_stack[stack_depth - 1], circ);
          else
            add_shape_to_scene(new_scene, circ);
        }
      }
    } else if (strncmp(line, "ELLIPSE", 7) == 0 && new_scene != NULL) {
      int cx, cy, rx, ry, mx, my, angle, ix, iy;
      float scale = 1.0f;
      char stroke[20], fill[20];
      if (sscanf(line, "ELLIPSE %d %d %d %d %19s %19s %d %d %d %d %d %f", &cx,
                 &cy, &rx, &ry, stroke, fill, &mx, &my, &angle, &ix, &iy,
                 &scale) == 12) {
        if (strcmp(stroke, "none") == 0)
          stroke[0] = '\0';
        if (strcmp(fill, "none") == 0)
          fill[0] = '\0';
        shape_t *ell = create_ellipse(new_scene, cx, cy, rx, ry, stroke, fill);
        if (ell) {
          apply_transform(ell, mx, my, angle, ix, iy);
          if (stack_depth > 0)
            add_shape_to_group(group_stack[stack_depth - 1], ell);
          else
            add_shape_to_scene(new_scene, ell);
        }
      }
    } else if (strncmp(line, "LINE", 4) == 0 && new_scene != NULL) {
      int x1, y1, x2, y2, mx, my, angle, ix, iy;
      float scale = 1.0f;
      char stroke[20], fill[20];
      if (sscanf(line, "LINE %d %d %d %d %19s %19s %d %d %d %d %d %f", &x1, &y1,
                 &x2, &y2, stroke, fill, &mx, &my, &angle, &ix, &iy,
                 &scale) == 12) {
        if (strcmp(stroke, "none") == 0)
          stroke[0] = '\0';
        if (strcmp(fill, "none") == 0)
          fill[0] = '\0';
        shape_t *line_shape =
            create_line(new_scene, x1, y1, x2, y2, stroke, fill);
        if (line_shape) {
          apply_transform(line_shape, mx, my, angle, ix, iy);
          if (stack_depth > 0)
            add_shape_to_group(group_stack[stack_depth - 1], line_shape);
          else
            add_shape_to_scene(new_scene, line_shape);
        }
      }
    } else if (strncmp(line, "POLYLINE", 8) == 0 && new_scene != NULL) {
      char stroke[20], fill[20];
      int mx, my, angle, ix, iy;
      float scale = 1.0f;
      size_t count;

      if (sscanf(line, "POLYLINE %19s %19s %d %d %d %d %d %zu %f", stroke, fill,
                 &mx, &my, &angle, &ix, &iy, &count, &scale) == 9) {

        if (strcmp(stroke, "none") == 0)
          stroke[0] = '\0';
        if (strcmp(fill, "none") == 0)
          fill[0] = '\0';

        shape_t *poly =
            create_point_list_shape(new_scene, POLYLINE, stroke, fill);
        if (poly != NULL) {
          char *ptr = line;
          int spaces = 0;
          while (*ptr && spaces < 9) {
            if (*ptr == ' ')
              spaces++;
            ptr++;
          }

          for (size_t i = 0; i < count; i++) {
            int x, y;
            if (sscanf(ptr, "%d,%d", &x, &y) == 2) {
              add_point(poly, x, y);
              while (*ptr && *ptr != ' ')
                ptr++;
              while (*ptr == ' ')
                ptr++;
            } else {
              break;
            }
          }

          apply_transform(poly, mx, my, angle, ix, iy);
          if (stack_depth > 0)
            add_shape_to_group(group_stack[stack_depth - 1], poly);
          else
            add_shape_to_scene(new_scene, poly);
        }
      }

    } else if (strncmp(line, "POLYGON", 7) == 0 && new_scene != NULL) {
      char stroke[20], fill[20];
      float scale = 1.0f;
      int mx, my, angle, ix, iy;
      size_t count;

      if (sscanf(line, "POLYGON %19s %19s %d %d %d %d %d %zu %f", stroke, fill,
                 &mx, &my, &angle, &ix, &iy, &count, &scale) == 9) {

        if (strcmp(stroke, "none") == 0)
          stroke[0] = '\0';
        if (strcmp(fill, "none") == 0)
          fill[0] = '\0';

        shape_t *poly =
            create_point_list_shape(new_scene, POLYGON, stroke, fill);
        if (poly != NULL) {
          char *ptr = line;
          int spaces = 0;
          while (*ptr && spaces < 9) {
            if (*ptr == ' ')
              spaces++;
            ptr++;
          }

          for (size_t i = 0; i < count; i++) {
            int x, y;
            if (sscanf(ptr, "%d,%d", &x, &y) == 2) {
              add_point(poly, x, y);
              while (*ptr && *ptr != ' ')
                ptr++;
              while (*ptr == ' ')
                ptr++;
            } else {
              break;
            }
          }

          apply_transform(poly, mx, my, angle, ix, iy);
          if (stack_depth > 0)
            add_shape_to_group(group_stack[stack_depth - 1], poly);
          else
            add_shape_to_scene(new_scene, poly);
        }
      }
    }
  }

  fclose(f);
  printf("> Project loaded successfully from '%s'.\n", filename);
  return new_scene;
}
