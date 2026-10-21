#define _POSIX_C_SOURCE 200809L
#include "../include/storage.h"
#include "../include/geometry.h"
#include "../include/group.h"
#include "../include/scene.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DEPTH 50
#define FILE_TAG "SCENE2"

static const char *or_none(const char *s) { return s[0] ? s : "none"; }

static void write_tail(FILE *f, const shape_t *s) {
  const transformation_t *t = &s->transform;
  fprintf(f, " %s %s %d %d %d %d %d %f\n", or_none(s->style.color_stroke),
          or_none(s->style.color_fill), t->move_x, t->move_y, t->angle,
          t->invert_x ? 1 : 0, t->invert_y ? 1 : 0, t->scale_factor);
}

static void save_shape(const shape_t *s, FILE *f) {
  const transformation_t *t = &s->transform;

  switch (s->type) {
  case GROUP:
    fprintf(f, "GROUP %d %d %d %d %d %d %f\n", s->id, t->move_x, t->move_y,
            t->angle, t->invert_x ? 1 : 0, t->invert_y ? 1 : 0,
            t->scale_factor);
    for (const shape_t *c = s->data.group.head; c != NULL; c = c->next)
      save_shape(c, f);
    fprintf(f, "END_GROUP\n");
    break;
  case RECTANGLE:
    fprintf(f, "RECT %d %d %d %d %d", s->id, s->data.rect.x, s->data.rect.y,
            s->data.rect.width, s->data.rect.height);
    write_tail(f, s);
    break;
  case CIRCLE:
    fprintf(f, "CIRCLE %d %d %d %d", s->id, s->data.circle.cx,
            s->data.circle.cy, s->data.circle.r);
    write_tail(f, s);
    break;
  case ELLIPSE:
    fprintf(f, "ELLIPSE %d %d %d %d %d", s->id, s->data.ellipse.cx,
            s->data.ellipse.cy, s->data.ellipse.rx, s->data.ellipse.ry);
    write_tail(f, s);
    break;
  case LINE:
    fprintf(f, "LINE %d %d %d %d %d", s->id, s->data.line.p1.x,
            s->data.line.p1.y, s->data.line.p2.x, s->data.line.p2.y);
    write_tail(f, s);
    break;
  case POLYLINE:
  case POLYGON: {

    fprintf(f, "%s %d %s %s %d %d %d %d %d %zu %f",
            s->type == POLYLINE ? "POLYLINE" : "POLYGON", s->id,
            or_none(s->style.color_stroke), or_none(s->style.color_fill),
            t->move_x, t->move_y, t->angle, t->invert_x ? 1 : 0,
            t->invert_y ? 1 : 0, s->data.point_list.count, t->scale_factor);
    for (const point_node_t *p = s->data.point_list.head; p; p = p->next)
      fprintf(f, " %d,%d", p->p.x, p->p.y);
    fprintf(f, "\n");
    break;
  }
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
  fprintf(f, FILE_TAG " %d %d %d\n", scene->view_width, scene->view_height,
          scene->next_id);

  for (shape_t *curr = scene->head; curr != NULL; curr = curr->next)
    save_shape(curr, f);

  if (fclose(f) != 0) {
    printf("Error: failed while writing '%s'.\n", filename);
    return;
  }
  printf("> Project saved successfully in '%s'.\n", filename);
}

static void apply_transform(shape_t *shape, int mx, int my, int angle, int ix,
                            int iy, float scale) {
  shape->transform.move_x = mx;
  shape->transform.move_y = my;
  shape->transform.angle = angle;
  shape->transform.invert_x = (ix != 0);
  shape->transform.invert_y = (iy != 0);
  shape->transform.scale_factor = (scale > 0.0f) ? scale : 1.0f;
}

typedef struct {
  char stroke[32];
  char fill[32];
  int mx, my, angle, ix, iy;
  float scale;
} tail_t;

static bool parse_tail(const char *p, tail_t *t) {
  int ok = sscanf(p, " %31s %31s %d %d %d %d %d %f", t->stroke, t->fill, &t->mx,
                  &t->my, &t->angle, &t->ix, &t->iy, &t->scale);
  if (ok != 8)
    return false;
  if (strcmp(t->stroke, "none") == 0)
    t->stroke[0] = '\0';
  if (strcmp(t->fill, "none") == 0)
    t->fill[0] = '\0';
  return true;
}

static void attach(scene_t *sc, shape_t **stack, int depth, shape_t *s) {
  if (depth > 0)
    add_shape_to_group(stack[depth - 1], s);
  else
    add_shape_to_scene(sc, s);
}

static void finish(scene_t *sc, shape_t **stack, int depth, shape_t *s, int id,
                   const tail_t *t) {
  if (s == NULL)
    return;
  s->id = id;
  apply_transform(s, t->mx, t->my, t->angle, t->ix, t->iy, t->scale);
  attach(sc, stack, depth, s);
}

static void load_poly(scene_t *sc, shape_t **stack, int depth,
                      shape_type_t type, const char *args) {
  int id, mx, my, angle, ix, iy, off = 0;
  size_t count;
  float scale;
  char stroke[32], fill[32];

  if (sscanf(args, " %d %31s %31s %d %d %d %d %d %zu %f%n", &id, stroke, fill,
             &mx, &my, &angle, &ix, &iy, &count, &scale, &off) != 10)
    return;
  if (strcmp(stroke, "none") == 0)
    stroke[0] = '\0';
  if (strcmp(fill, "none") == 0)
    fill[0] = '\0';

  shape_t *poly = create_point_list_shape(sc, type, stroke, fill);
  if (poly == NULL)
    return;

  const char *ptr = args + off;
  for (size_t i = 0; i < count; i++) {
    int x, y, n = 0;
    if (sscanf(ptr, " %d,%d%n", &x, &y, &n) != 2)
      break;
    add_point(poly, x, y);
    ptr += n;
  }

  poly->id = id;
  apply_transform(poly, mx, my, angle, ix, iy, scale);
  attach(sc, stack, depth, poly);
}

static int max_id(const shape_t *s) {
  int m = 0;
  for (; s != NULL; s = s->next) {
    if (s->id > m)
      m = s->id;
    if (s->type == GROUP) {
      int c = max_id(s->data.group.head);
      if (c > m)
        m = c;
    }
  }
  return m;
}

scene_t *load_project(const char *filename) {
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    printf("Error: unable to read %s.\n", filename);
    return NULL;
  }

  char *line = NULL;
  size_t cap = 0;
  scene_t *sc = NULL;
  int saved_next_id = 1;
  shape_t *stack[MAX_DEPTH];
  int depth = 0;
  bool bad = false;

  while (!bad && getline(&line, &cap, f) != -1) {
    line[strcspn(line, "\r\n")] = '\0';

    if (strncmp(line, FILE_TAG " ", sizeof(FILE_TAG)) == 0) {
      int w, h, nid;
      if (sc != NULL) {
        free_scene(sc);
        sc = NULL;
        depth = 0;
      }
      if (sscanf(line, FILE_TAG " %d %d %d", &w, &h, &nid) == 3) {
        sc = create_scene(w, h);
        saved_next_id = nid;
      }
    } else if (sc == NULL) {
      continue;
    } else if (strncmp(line, "END_GROUP", 9) == 0) {
      if (depth > 0)
        depth--;
    } else if (strncmp(line, "GROUP ", 6) == 0) {
      int id, mx, my, angle, ix, iy;
      float scale;
      if (sscanf(line, "GROUP %d %d %d %d %d %d %f", &id, &mx, &my, &angle, &ix,
                 &iy, &scale) == 7) {
        if (depth >= MAX_DEPTH) {
          bad = true;
          break;
        }
        shape_t *g = create_group(sc);
        if (g) {
          g->id = id;
          apply_transform(g, mx, my, angle, ix, iy, scale);
          attach(sc, stack, depth, g);
          stack[depth++] = g;
        }
      }
    } else if (strncmp(line, "RECT ", 5) == 0) {
      int id, x, y, w, h, off = 0;
      tail_t t;
      if (sscanf(line, "RECT %d %d %d %d %d%n", &id, &x, &y, &w, &h, &off) ==
              5 &&
          parse_tail(line + off, &t))
        finish(sc, stack, depth,
               create_rectangle(sc, x, y, w, h, t.stroke, t.fill), id, &t);
    } else if (strncmp(line, "CIRCLE ", 7) == 0) {
      int id, cx, cy, r, off = 0;
      tail_t t;
      if (sscanf(line, "CIRCLE %d %d %d %d%n", &id, &cx, &cy, &r, &off) == 4 &&
          parse_tail(line + off, &t))
        finish(sc, stack, depth, create_circle(sc, cx, cy, r, t.stroke, t.fill),
               id, &t);
    } else if (strncmp(line, "ELLIPSE ", 8) == 0) {
      int id, cx, cy, rx, ry, off = 0;
      tail_t t;
      if (sscanf(line, "ELLIPSE %d %d %d %d %d%n", &id, &cx, &cy, &rx, &ry,
                 &off) == 5 &&
          parse_tail(line + off, &t))
        finish(sc, stack, depth,
               create_ellipse(sc, cx, cy, rx, ry, t.stroke, t.fill), id, &t);
    } else if (strncmp(line, "LINE ", 5) == 0) {
      int id, x1, y1, x2, y2, off = 0;
      tail_t t;
      if (sscanf(line, "LINE %d %d %d %d %d%n", &id, &x1, &y1, &x2, &y2,
                 &off) == 5 &&
          parse_tail(line + off, &t))
        finish(sc, stack, depth,
               create_line(sc, x1, y1, x2, y2, t.stroke, t.fill), id, &t);
    } else if (strncmp(line, "POLYLINE ", 9) == 0) {
      load_poly(sc, stack, depth, POLYLINE, line + 8);
    } else if (strncmp(line, "POLYGON ", 8) == 0) {
      load_poly(sc, stack, depth, POLYGON, line + 7);
    }
  }

  free(line);
  fclose(f);

  if (bad || sc == NULL) {
    if (sc)
      free_scene(sc);
    printf("Error: '%s' is not a valid project file.\n", filename);
    return NULL;
  }

  int m = max_id(sc->head);
  sc->next_id = (saved_next_id > m) ? saved_next_id : m + 1;

  printf("> Project loaded successfully from '%s'.\n", filename);
  return sc;
}
