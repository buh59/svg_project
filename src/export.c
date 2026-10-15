#include "../include/types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void get_local_center(shape_t *shape, int *cx, int *cy) {
  switch (shape->type) {
  case RECTANGLE:
    *cx = shape->data.rect.x + shape->data.rect.width / 2;
    *cy = shape->data.rect.y + shape->data.rect.height / 2;
    break;
  case CIRCLE:
    *cx = shape->data.circle.cx;
    *cy = shape->data.circle.cy;
    break;
  case ELLIPSE:
    *cx = shape->data.ellipse.cx;
    *cy = shape->data.ellipse.cy;
    break;
  case LINE:
    *cx = (shape->data.line.p1.x + shape->data.line.p2.x) / 2;
    *cy = (shape->data.line.p1.y + shape->data.line.p2.y) / 2;
    break;
  case POLYLINE: {
    point_node_t *pt = shape->data.polyline.head;
    int sum_x = 0, sum_y = 0;
    size_t count = 0;
    while (pt) {
      sum_x += pt->p.x;
      sum_y += pt->p.y;
      count++;
      pt = pt->next;
    }
    *cx = (count > 0) ? sum_x / count : 0;
    *cy = (count > 0) ? sum_y / count : 0;
    break;
  }
  case POLYGON: {
    point_node_t *pt = shape->data.polygon.head;
    int sum_x = 0, sum_y = 0;
    size_t count = 0;
    while (pt) {
      sum_x += pt->p.x;
      sum_y += pt->p.y;
      count++;
      pt = pt->next;
    }
    *cx = (count > 0) ? sum_x / count : 0;
    *cy = (count > 0) ? sum_y / count : 0;
    break;
  }
  default:
    *cx = 0;
    *cy = 0;
  }
}

static void write_transform(FILE *f, shape_t *shape) {
  int sx = shape->transform.invert_x ? -1 : 1;
  int sy = shape->transform.invert_y ? -1 : 1;
  int cx, cy;
  get_local_center(shape, &cx, &cy);
  int cx_final = cx + shape->transform.move_x;
  int cy_final = cy + shape->transform.move_y;

  fprintf(
      f,
      "transform=\"translate(%d %d) rotate(%d) scale(%d %d) translate(%d %d)\"",
      cx_final, cy_final, shape->transform.angle, sx, sy, -cx, -cy);
}

static void export_shape_recursive(shape_t *shape, FILE *f, int depth) {
  if (shape == NULL)
    return;

  for (int i = 0; i < depth; i++)
    fprintf(f, "  ");

  if (shape->type == GROUP) {
    int sx = shape->transform.invert_x ? -1 : 1;
    int sy = shape->transform.invert_y ? -1 : 1;
    fprintf(f,
            "<g id=\"group_%d\" transform=\"translate(%d %d) rotate(%d) "
            "scale(%d %d)\">\n",
            shape->id, shape->transform.move_x, shape->transform.move_y,
            shape->transform.angle, sx, sy);

    shape_t *child = shape->data.group.head;
    while (child != NULL) {
      export_shape_recursive(child, f, depth + 1);
      child = child->next;
    }
    for (int i = 0; i < depth; i++)
      fprintf(f, "  ");
    fprintf(f, "</g>\n");
  } else {
    switch (shape->type) {
    case RECTANGLE:
      fprintf(f,
              "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" "
              "fill=\"%s\" stroke=\"%s\" stroke-width=\"3\" ",
              shape->data.rect.x, shape->data.rect.y, shape->data.rect.width,
              shape->data.rect.height, shape->style.color_fill,
              shape->style.color_stroke);
      write_transform(f, shape);
      fprintf(f, " />\n");
      break;
    case CIRCLE:
      fprintf(f,
              "<circle cx=\"%d\" cy=\"%d\" r=\"%d\" "
              "fill=\"%s\" stroke=\"%s\" stroke-width=\"3\" ",
              shape->data.circle.cx, shape->data.circle.cy,
              shape->data.circle.r, shape->style.color_fill,
              shape->style.color_stroke);
      write_transform(f, shape);
      fprintf(f, " />\n");
      break;
    case ELLIPSE:
      fprintf(f,
              "<ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" "
              "fill=\"%s\" stroke=\"%s\" stroke-width=\"3\" ",
              shape->data.ellipse.cx, shape->data.ellipse.cy,
              shape->data.ellipse.rx, shape->data.ellipse.ry,
              shape->style.color_fill, shape->style.color_stroke);
      write_transform(f, shape);
      fprintf(f, " />\n");
      break;
    case LINE:
      fprintf(f,
              "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" "
              "stroke=\"%s\" stroke-width=\"3\" ",
              shape->data.line.p1.x, shape->data.line.p1.y,
              shape->data.line.p2.x, shape->data.line.p2.y,
              shape->style.color_stroke);
      write_transform(f, shape);
      fprintf(f, " />\n");
      break;
    case POLYLINE: {
      fprintf(f, "<polyline points=\"");
      point_node_t *pt = shape->data.polyline.head;
      while (pt) {
        fprintf(f, "%d,%d ", pt->p.x, pt->p.y);
        pt = pt->next;
      }
      fprintf(f, "\" fill=\"%s\" stroke=\"%s\" stroke-width=\"3\" ",
              shape->style.color_fill, shape->style.color_stroke);
      write_transform(f, shape);
      fprintf(f, " />\n");
      break;
    }
    case POLYGON: {
      fprintf(f, "<polygon points=\"");
      point_node_t *pt = shape->data.polygon.head;
      while (pt) {
        fprintf(f, "%d,%d ", pt->p.x, pt->p.y);
        pt = pt->next;
      }
      fprintf(f, "\" fill=\"%s\" stroke=\"%s\" stroke-width=\"3\" ",
              shape->style.color_fill, shape->style.color_stroke);
      write_transform(f, shape);
      fprintf(f, " />\n");
      break;
    }
    default:
      break;
    }
  }

  export_shape_recursive(shape->next, f, depth);
}

void export_to_svg(scene_t *scene, const char *filename) {
  if (scene == NULL)
    return;
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Erreur : impossible de creer le fichier %s\n", filename);
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

  if (scene->head != NULL) {
    export_shape_recursive(scene->head, f, 1);
  }

  fprintf(f, "</svg>\n");
  fclose(f);
  printf("> Rendu SVG genere dans %s.\n", filename);

  char cmd[100];
  sprintf(cmd, "xdg-open %s &", filename);
  system(cmd);
}
