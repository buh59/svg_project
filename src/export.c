#define _POSIX_C_SOURCE 200809L
#include "../include/export.h"
#include "../include/geometry.h"
#include "../include/types.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

static const char *paint(const char *c) { return c[0] ? c : "none"; }

static void write_transform(FILE *f, const shape_t *shape) {
  float sx = (shape->transform.invert_x ? -1.0f : 1.0f) *
             shape->transform.scale_factor;
  float sy = (shape->transform.invert_y ? -1.0f : 1.0f) *
             shape->transform.scale_factor;

  int cx, cy;
  get_local_center((shape_t *)shape, &cx, &cy);
  int cx_final = cx + shape->transform.move_x;
  int cy_final = cy + shape->transform.move_y;

  fprintf(f,
          "transform=\"translate(%d %d) rotate(%d) scale(%.2f %.2f) "
          "translate(%d %d)\"",
          cx_final, cy_final, shape->transform.angle, sx, sy, -cx, -cy);
}

static void indent(FILE *f, int depth) {
  for (int i = 0; i < depth; i++)
    fprintf(f, "  ");
}

static void export_shape(const shape_t *shape, FILE *f, int depth) {
  indent(f, depth);

  switch (shape->type) {
  case GROUP: {

    float sx = (shape->transform.invert_x ? -1.0f : 1.0f) *
               shape->transform.scale_factor;
    float sy = (shape->transform.invert_y ? -1.0f : 1.0f) *
               shape->transform.scale_factor;
    fprintf(f,
            "<g id=\"group_%d\" transform=\"translate(%d %d) rotate(%d) "
            "scale(%.2f %.2f)\">\n",
            shape->id, shape->transform.move_x, shape->transform.move_y,
            shape->transform.angle, sx, sy);
    for (const shape_t *c = shape->data.group.head; c != NULL; c = c->next)
      export_shape(c, f, depth + 1);
    indent(f, depth);
    fprintf(f, "</g>\n");
    break;
  }
  case RECTANGLE:
    fprintf(f,
            "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
            "fill=\"%s\" stroke=\"%s\" stroke-width=\"3\" ",
            shape->data.rect.x, shape->data.rect.y, shape->data.rect.width,
            shape->data.rect.height, paint(shape->style.color_fill),
            paint(shape->style.color_stroke));
    write_transform(f, shape);
    fprintf(f, " />\n");
    break;
  case CIRCLE:
    fprintf(f,
            "<circle cx=\"%d\" cy=\"%d\" r=\"%d\" "
            "fill=\"%s\" stroke=\"%s\" stroke-width=\"3\" ",
            shape->data.circle.cx, shape->data.circle.cy, shape->data.circle.r,
            paint(shape->style.color_fill), paint(shape->style.color_stroke));
    write_transform(f, shape);
    fprintf(f, " />\n");
    break;
  case ELLIPSE:
    fprintf(f,
            "<ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" "
            "fill=\"%s\" stroke=\"%s\" stroke-width=\"3\" ",
            shape->data.ellipse.cx, shape->data.ellipse.cy,
            shape->data.ellipse.rx, shape->data.ellipse.ry,
            paint(shape->style.color_fill), paint(shape->style.color_stroke));
    write_transform(f, shape);
    fprintf(f, " />\n");
    break;
  case LINE:
    fprintf(f,
            "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" "
            "stroke=\"%s\" stroke-width=\"3\" ",
            shape->data.line.p1.x, shape->data.line.p1.y, shape->data.line.p2.x,
            shape->data.line.p2.y, paint(shape->style.color_stroke));
    write_transform(f, shape);
    fprintf(f, " />\n");
    break;
  case POLYLINE:
  case POLYGON: {
    const char *tag = (shape->type == POLYLINE) ? "polyline" : "polygon";
    fprintf(f, "<%s points=\"", tag);
    for (const point_node_t *pt = shape->data.point_list.head; pt;
         pt = pt->next)
      fprintf(f, "%d,%d ", pt->p.x, pt->p.y);
    fprintf(f, "\" fill=\"%s\" stroke=\"%s\" stroke-width=\"3\" ",
            paint(shape->style.color_fill), paint(shape->style.color_stroke));
    write_transform(f, shape);
    fprintf(f, " />\n");
    break;
  }
  }
}

void export_to_svg(scene_t *scene, const char *filename) {
  if (scene == NULL)
    return;
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Error: could not create file %s\n", filename);
    return;
  }

  fprintf(f,
          "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" "
          "height=\"%d\" viewBox=\"0 0 %d %d\">\n",
          scene->view_width, scene->view_height, scene->view_width,
          scene->view_height);
  fprintf(
      f,
      "  <rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"white\" />\n",
      scene->view_width, scene->view_height);

  for (const shape_t *s = scene->head; s != NULL; s = s->next)
    export_shape(s, f, 1);

  fprintf(f, "</svg>\n");
  if (fclose(f) != 0) {
    printf("Error: failed while writing %s\n", filename);
    return;
  }
  printf("> SVG render generated in %s.\n", filename);
}

void open_in_viewer(const char *filename) {
  signal(SIGCHLD, SIG_IGN);
  pid_t pid = fork();
  if (pid == 0) {
    execlp("xdg-open", "xdg-open", filename, (char *)NULL);
    _exit(127);
  }
}
