#include "../include/geometry.h"
#include "../include/group.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_local_center(shape_t *shape, int *cx, int *cy) {
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
  case POLYLINE:
  case POLYGON: {
    point_node_t *pt = shape->data.point_list.head;
    int sum_x = 0, sum_y = 0;
    size_t count = 0;
    while (pt) {
      sum_x += pt->p.x;
      sum_y += pt->p.y;
      count++;
      pt = pt->next;
    }
    *cx = (count > 0) ? sum_x / (int)count : 0;
    *cy = (count > 0) ? sum_y / (int)count : 0;
    break;
  }
  default:
    *cx = 0;
    *cy = 0;
  }
}

shape_t *create_base_shape(scene_t *scene, shape_type_t type,
                           char *color_stroke, char *color_fill) {
  shape_t *shape = malloc(sizeof(struct shape_s));
  if (shape == NULL) {
    fprintf(stderr, "Error: Failed to allocate shape.\n");
    return NULL;
  }

  shape->prev = NULL;
  shape->next = NULL;
  shape->parent = NULL;

  if (type == GROUP) {
    shape->data.group.head = NULL;
    shape->data.group.tail = NULL;
  }

  shape->id = scene->next_id++;
  shape->type = type;

  shape->transform.move_x = 0;
  shape->transform.move_y = 0;
  shape->transform.angle = 0;
  shape->transform.invert_x = false;
  shape->transform.invert_y = false;
  shape->transform.scale_factor = 1.0f;

  if (color_fill != NULL) {
    strncpy(shape->style.color_fill, color_fill, 31);
    shape->style.color_fill[31] = '\0';
  } else {
    shape->style.color_fill[0] = '\0';
  }

  if (color_stroke != NULL) {
    strncpy(shape->style.color_stroke, color_stroke, 31);
    shape->style.color_stroke[31] = '\0';
  } else {
    shape->style.color_stroke[0] = '\0';
  }

  return shape;
}

shape_t *create_rectangle(scene_t *scene, int x, int y, int width, int height,
                          char *color_stroke, char *color_fill) {
  shape_t *shape =
      create_base_shape(scene, RECTANGLE, color_stroke, color_fill);
  if (shape == NULL)
    return NULL;

  shape->data.rect.x = x;
  shape->data.rect.y = y;
  shape->data.rect.width = width;
  shape->data.rect.height = height;

  return shape;
}

shape_t *create_circle(scene_t *scene, int cx, int cy, int r,
                       char *color_stroke, char *color_fill) {
  shape_t *shape = create_base_shape(scene, CIRCLE, color_stroke, color_fill);
  if (shape == NULL)
    return NULL;

  shape->data.circle.cx = cx;
  shape->data.circle.cy = cy;
  shape->data.circle.r = r;

  return shape;
}

shape_t *create_ellipse(scene_t *scene, int cx, int cy, int rx, int ry,
                        char *color_stroke, char *color_fill) {
  shape_t *shape = create_base_shape(scene, ELLIPSE, color_stroke, color_fill);
  if (shape == NULL)
    return NULL;

  shape->data.ellipse.cx = cx;
  shape->data.ellipse.cy = cy;
  shape->data.ellipse.rx = rx;
  shape->data.ellipse.ry = ry;

  return shape;
}

shape_t *create_line(scene_t *scene, int x1, int y1, int x2, int y2,
                     char *color_stroke, char *color_fill) {
  shape_t *shape = create_base_shape(scene, LINE, color_stroke, color_fill);
  if (shape == NULL)
    return NULL;

  shape->data.line.p1.x = x1;
  shape->data.line.p1.y = y1;
  shape->data.line.p2.x = x2;
  shape->data.line.p2.y = y2;

  return shape;
}

shape_t *create_point_list_shape(scene_t *scene, shape_type_t type,
                                 char *color_stroke, char *color_fill) {
  if (type != POLYLINE && type != POLYGON)
    return NULL;

  shape_t *shape = create_base_shape(scene, type, color_stroke, color_fill);
  if (shape != NULL) {
    shape->data.point_list.head = NULL;
    shape->data.point_list.tail = NULL;
    shape->data.point_list.count = 0;
  }
  return shape;
}

bool add_point(shape_t *shape, int x, int y) {
  if (shape == NULL || (shape->type != POLYLINE && shape->type != POLYGON))
    return false;

  point_node_t *new_node = malloc(sizeof(point_node_t));
  if (new_node == NULL)
    return false;

  new_node->p.x = x;
  new_node->p.y = y;
  new_node->next = NULL;

  if (shape->data.point_list.tail == NULL) {
    shape->data.point_list.head = new_node;
    shape->data.point_list.tail = new_node;
  } else {
    shape->data.point_list.tail->next = new_node;
    shape->data.point_list.tail = new_node;
  }
  shape->data.point_list.count++;
  return true;
}

shape_t *create_steamhappy(scene_t *scene, int cx, int cy, int size) {
  shape_t *group = create_group(scene);
  if (!group)
    return NULL;

  // L'ellipse du corps fait 32 de rayon (64 de large).
  // Ce calcul permet à `size=100` d'avoir une largeur d'environ 100 pixels.
  float s = (float)size / 64.0f;

  // ==========================================
  // CALQUE 1 : ARRIÈRE-PLAN (Bras & Jambes)
  // ==========================================
  shape_t *leg_r =
      create_ellipse(scene, cx + (int)(5 * s), cy + (int)(36 * s), (int)(4 * s),
                     (int)(9 * s), "black", "#404017");
  if (leg_r) {
    leg_r->transform.angle = 20;
    add_shape_to_group(group, leg_r);
  }

  shape_t *leg_l =
      create_ellipse(scene, cx - (int)(8 * s), cy + (int)(36 * s), (int)(4 * s),
                     (int)(9 * s), "black", "#edc40c");
  if (leg_l) {
    leg_l->transform.angle = -17;
    add_shape_to_group(group, leg_l);
  }

  shape_t *arm_l =
      create_ellipse(scene, cx - (int)(33 * s), cy - (int)(4 * s),
                     (int)(10 * s), (int)(5 * s), "#00000f", "#edc40c");
  if (arm_l) {
    arm_l->transform.angle = 46;
    add_shape_to_group(group, arm_l);
  }

  shape_t *arm_r =
      create_ellipse(scene, cx + (int)(33 * s), cy - (int)(4 * s),
                     (int)(10 * s), (int)(5 * s), "black", "#404017");
  if (arm_r) {
    arm_r->transform.angle = -46;
    add_shape_to_group(group, arm_r);
  }

  // ==========================================
  // CALQUE 2 : CORPS PRINCIPAL
  // ==========================================
  shape_t *body = create_ellipse(scene, cx, cy, (int)(32 * s), (int)(38 * s),
                                 "black", "#edc40c");
  if (body)
    add_shape_to_group(group, body);

  // ==========================================
  // CALQUE 3 : BOUCHE, DENTS, LANGUE & JOUES
  // ==========================================
  shape_t *mouth =
      create_ellipse(scene, cx + (int)(1 * s), cy + (int)(1 * s), (int)(11 * s),
                     (int)(20 * s), "#000000", "black");
  if (mouth) {
    mouth->transform.angle = 4;
    add_shape_to_group(group, mouth);
  }

  shape_t *tongue1 =
      create_ellipse(scene, cx - (int)(2 * s), cy + (int)(12 * s), (int)(3 * s),
                     (int)(8 * s), "#d35f5f", "#d35f5f");
  if (tongue1)
    add_shape_to_group(group, tongue1);

  shape_t *tongue2 =
      create_ellipse(scene, cx + (int)(2 * s), cy + (int)(13 * s), (int)(3 * s),
                     (int)(7 * s), "#d35f5f", "#d35f5f");
  if (tongue2)
    add_shape_to_group(group, tongue2);

  shape_t *tongue3 = create_ellipse(scene, cx, cy + (int)(19 * s), (int)(2 * s),
                                    (int)(1 * s), "#d35f5f", "#d35f5f");
  if (tongue3)
    add_shape_to_group(group, tongue3);

  // Dents du bas
  shape_t *tb1 = create_ellipse(scene, cx + (int)(2 * s), cy + (int)(19 * s),
                                (int)(1 * s), (int)(1.4 * s), "none", "white");
  shape_t *tb2 =
      create_ellipse(scene, cx + (int)(1 * s), cy + (int)(19 * s),
                     (int)(0.9 * s), (int)(1.3 * s), "none", "white");
  shape_t *tb3 = create_ellipse(scene, cx - (int)(6 * s), cy + (int)(16 * s),
                                (int)(1 * s), (int)(0.8 * s), "none", "white");
  shape_t *tb4 = create_ellipse(scene, cx - (int)(5 * s), cy + (int)(16 * s),
                                (int)(0.4 * s), (int)(1 * s), "none", "white");
  if (tb1)
    add_shape_to_group(group, tb1);
  if (tb2)
    add_shape_to_group(group, tb2);
  if (tb3) {
    tb3->transform.angle = -57;
    add_shape_to_group(group, tb3);
  }
  if (tb4) {
    tb4->transform.angle = -29;
    add_shape_to_group(group, tb4);
  }

  // Dents du haut
  shape_t *tt1 = create_ellipse(scene, cx + (int)(1 * s), cy - (int)(17 * s),
                                (int)(1.5 * s), (int)(2 * s), "none", "white");
  shape_t *tt2 =
      create_ellipse(scene, cx + (int)(2 * s), cy - (int)(18 * s),
                     (int)(2.4 * s), (int)(1.4 * s), "none", "white");
  shape_t *tt3 =
      create_ellipse(scene, cx + (int)(4 * s), cy - (int)(18 * s),
                     (int)(1.4 * s), (int)(1.2 * s), "none", "white");
  if (tt1)
    add_shape_to_group(group, tt1);
  if (tt2)
    add_shape_to_group(group, tt2);
  if (tt3)
    add_shape_to_group(group, tt3);

  // Joues
  shape_t *cheek_l =
      create_ellipse(scene, cx - (int)(9 * s), cy - (int)(17 * s), (int)(6 * s),
                     (int)(5 * s), "black", "#edc40c");
  if (cheek_l)
    add_shape_to_group(group, cheek_l);

  shape_t *cheek_r =
      create_ellipse(scene, cx + (int)(13 * s), cy - (int)(16 * s),
                     (int)(5 * s), (int)(4 * s), "black", "#edc40c");
  if (cheek_r) {
    cheek_r->transform.angle = -21;
    add_shape_to_group(group, cheek_r);
  }

  // ==========================================
  // CALQUE 4 : YEUX
  // ==========================================
  shape_t *eye_l =
      create_ellipse(scene, cx - (int)(8 * s), cy - (int)(35 * s),
                     (int)(10 * s), (int)(13 * s), "#000000", "#e8e8e8");
  if (eye_l)
    add_shape_to_group(group, eye_l);

  shape_t *eye_r =
      create_ellipse(scene, cx + (int)(8 * s), cy - (int)(36 * s),
                     (int)(10 * s), (int)(12 * s), "#000000", "#e8e8e8");
  if (eye_r)
    add_shape_to_group(group, eye_r);

  shape_t *pupil_l =
      create_ellipse(scene, cx - (int)(5 * s), cy - (int)(33 * s), (int)(4 * s),
                     (int)(5 * s), "black", "black");
  if (pupil_l)
    add_shape_to_group(group, pupil_l);

  shape_t *pupil_r =
      create_ellipse(scene, cx + (int)(8 * s), cy - (int)(34 * s), (int)(4 * s),
                     (int)(5 * s), "black", "black");
  if (pupil_r)
    add_shape_to_group(group, pupil_r);

  shape_t *sparkle_l =
      create_ellipse(scene, cx - (int)(8 * s), cy - (int)(38 * s), (int)(1 * s),
                     (int)(3 * s), "none", "white");
  if (sparkle_l) {
    sparkle_l->transform.angle = -28;
    add_shape_to_group(group, sparkle_l);
  }

  shape_t *sparkle_r =
      create_ellipse(scene, cx + (int)(7 * s), cy - (int)(39 * s), (int)(1 * s),
                     (int)(3 * s), "none", "white");
  if (sparkle_r) {
    sparkle_r->transform.angle = 4;
    add_shape_to_group(group, sparkle_r);
  }

  return group;
}
