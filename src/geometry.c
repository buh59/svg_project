#define _POSIX_C_SOURCE 200809L

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

  float s = (float)size / 360.0f;

  group->transform.scale_factor = s;
  group->transform.move_x = cx - (int)(180.0f * s);
  group->transform.move_y = cy - (int)(180.0f * s);

  shape_t *elem = NULL;

  add_shape_to_group(
      group,
      create_path(scene,
                  "m 219.97621,311.66547 c -2.05034,5.99332 -6.15104,12.4598 "
                  "-8.20138,14.98331 -2.05035,2.5235 -7.88595,7.88594 "
                  "-8.98999,8.51682 -1.10403,0.63088 -5.36244,2.05035 "
                  "-6.15104,1.73491 -0.78859,-0.31544 -4.10069,-0.63088 "
                  "-4.73157,-1.73491 -0.63087,-1.10403 -3.15438,-5.04701 "
                  "-3.31209,-5.99332 -0.15772,-0.94631 -1.10404,-7.57051 "
                  "-1.41947,-8.67454 -0.31544,-1.10404 0,-5.83561 0,-5.83561 z",
                  "#000000", "#404117"));
  add_shape_to_group(
      group,
      create_path(
          scene,
          "m 70.342667,146.52094 c -3.312098,-2.05035 -9.384279,-4.41613 "
          "-11.592345,-4.96815 -2.208066,-0.55202 -5.047008,-1.10403 "
          "-7.176214,-0.94631 -2.129206,0.15771 -5.520164,1.10403 "
          "-6.545338,1.7349 -1.025173,0.63088 -2.444644,2.52351 "
          "-2.838942,3.39096 -0.394297,0.86746 -0.946313,2.44465 "
          "-0.867454,4.33727 0.07886,1.89263 0.788595,6.93964 1.182892,7.72823 "
          "0.394298,0.7886 4.337273,8.51683 4.889289,9.69972 0.552016,1.18289 "
          "13.95813,17.97997 13.95813,17.97997 z",
          "#000000", "#fad103"));
  add_shape_to_group(
      group,
      create_path(
          scene,
          "m 286.70978,144.0801 c 3.3121,-2.05035 9.38428,-4.41613 "
          "11.59235,-4.96815 2.20806,-0.55202 5.04701,-1.10403 "
          "7.17621,-0.94631 2.12921,0.15771 5.52017,1.10403 6.54534,1.7349 "
          "1.02517,0.63088 2.44464,2.52351 2.83894,3.39096 0.3943,0.86746 "
          "0.94632,2.44465 0.86746,4.33727 -0.0789,1.89263 -0.7886,6.93964 "
          "-1.1829,7.72823 -0.39429,0.7886 -4.33727,8.51683 -4.88928,9.69972 "
          "-0.55202,1.18289 -13.95814,17.97997 -13.95814,17.97997 z",
          "#000000", "#404117"));
  add_shape_to_group(
      group,
      create_path(
          scene,
          "m 143.52428,310.39096 c 2.05034,5.99332 6.15104,12.4598 "
          "8.20138,14.98331 2.05035,2.5235 7.88595,7.88594 8.98999,8.51682 "
          "1.10403,0.63088 5.36244,2.05035 6.15104,1.73491 0.78859,-0.31544 "
          "4.10069,-0.63088 4.73157,-1.73491 0.63087,-1.10403 3.15438,-5.04701 "
          "3.31209,-5.99332 0.15772,-0.94631 1.10404,-7.57051 1.41947,-8.67454 "
          "0.31544,-1.10404 0,-5.83561 0,-5.83561 z",
          "#000000", "#e8b913"));

  add_shape_to_group(
      group,
      create_path(
          scene,
          "M 100.37175,78.513011 C 87.434944,96.356877 88.480351,93.685077 "
          "84.852814,101.09787 c -3.627537,7.41279 -2.523504,0.78859 "
          "-6.387619,12.22322 -3.864115,11.43463 -6.308759,21.84408 "
          "-7.412792,30.51862 -1.104033,8.67455 -6.939636,26.26022 "
          "-7.964809,35.32906 -1.025173,9.06884 -4.416132,9.69971 "
          "-0.157719,33.43642 4.258413,23.73671 3.785256,22.31724 "
          "7.491652,32.96327 3.706396,10.64603 15.062163,28.2317 "
          "20.661187,33.12099 5.599024,4.88928 23.263546,18.76856 "
          "32.805546,23.57898 9.542,4.81043 21.52864,8.98999 25.23504,9.69972 "
          "3.7064,0.70974 41.24351,1.81377 46.36938,1.81377 5.12587,0 "
          "34.69818,-5.20473 39.66633,-7.01849 4.96814,-1.81377 "
          "26.81222,-14.35243 32.25353,-19.79374 5.4413,-5.4413 "
          "18.92628,-22.71153 21.13434,-29.49345 2.20807,-6.78191 "
          "8.83227,-26.10249 9.46314,-33.04212 0.63088,-6.93964 "
          "0.15772,-35.6445 -1.57719,-45.18649 -1.73491,-9.542 "
          "-6.15104,-30.04547 -9.85743,-40.77036 -3.7064,-10.72489 "
          "-13.40612,-30.7552 -18.13769,-37.93142 -4.73157,-7.176211 "
          "-13.40611,-18.453118 -20.26689,-24.36758 -6.86077,-5.914462 "
          "-15.69304,-9.38428 -28.62599,-13.563833 -12.93296,-4.179553 "
          "-59.61778,0 -73.02389,1.57719 -13.40612,1.57719 -35.2502,6.860776 "
          "-38.01028,8.359106 -2.76008,1.49833 -8.13891,5.962278 "
          "-8.13891,5.962278 z",
          "#000000", "#f1c80a"));

  add_shape_to_group(
      group, create_path(
                 scene,
                 "m 236.43123,133.3829 c -14.49814,-4.68402 -20.07435,-8.02974 "
                 "-32.78811,-9.59108 -12.71375,-1.56134 -16.50557,-2.89963 "
                 "-24.75836,-2.00743 -8.25279,0.89219 -26.7658,4.46096 "
                 "-30.78067,5.5762 -4.01487,1.11525 -8.92193,2.00744 "
                 "-8.92193,2.00744 0,0 -0.66915,-2.23048 -1.78439,14.49814 "
                 "-1.11524,16.72863 -1.11524,48.40149 0.22305,58.43866 "
                 "1.33829,10.03718 3.79182,31.67287 10.48327,45.94796 "
                 "6.69145,14.27509 16.50558,24.08922 18.95911,25.8736 "
                 "2.45353,1.78439 8.92193,4.01487 11.82156,3.34573 "
                 "2.89963,-0.66915 8.02974,-2.23048 10.92937,-4.46097 "
                 "2.89962,-2.23048 11.15241,-9.81412 13.82899,-14.94424 "
                 "2.67658,-5.13011 7.3606,-14.05204 10.26023,-22.08178 "
                 "2.89962,-8.02974 10.70632,-32.7881 12.71375,-39.47955 "
                 "2.00744,-6.69145 9.81413,-63.12268 9.81413,-63.12268 z",
                 "#000000", "#000000"));

  add_shape_to_group(
      group, create_ellipse(scene, 206, 63, 45, 54, "#000000", "#e7e7e7"));

  elem = create_ellipse(scene, 145, 74, 42, 50, "#000000", "#e7e7e7");
  elem->transform.angle = -9;
  add_shape_to_group(group, elem);

  add_shape_to_group(
      group, create_ellipse(scene, 203, 64, 19, 24, "#000000", "#000000"));

  elem = create_ellipse(scene, 202, 41, 4, 14, "none", "#ffffff");
  elem->transform.angle = 1;
  add_shape_to_group(group, elem);

  elem = create_ellipse(scene, 145, 78, 18, 24, "#000000", "#000000");
  elem->transform.angle = -23;
  add_shape_to_group(group, elem);

  elem = create_ellipse(scene, 138, 60, 5, 14, "none", "#ffffff");
  elem->transform.angle = -31;
  add_shape_to_group(group, elem);

  add_shape_to_group(
      group,
      create_path(
          scene,
          "m 212.34201,240.66915 c -1.56134,-6.9145 -6.4684,-22.75093 "
          "-8.92194,-27.88105 -2.45353,-5.13011 -9.59107,-14.05204 "
          "-12.9368,-15.61338 -3.34572,-1.56134 -6.69145,-2.89963 "
          "-9.14498,-1.33829 -2.45353,1.56134 -3.12268,3.34573 -3.6803,2.78811 "
          "-0.55762,-0.55763 -1.28253,-1.78439 -3.2342,-2.67658 "
          "-1.95167,-0.8922 -4.18215,-1.72863 -5.79925,-1.6171 -1.6171,0.11152 "
          "-7.41636,0.61338 -8.5316,1.78438 -1.11524,1.17101 -7.30484,9.08922 "
          "-8.30855,12.2119 -1.00372,3.12267 -8.47584,21.97026 "
          "-8.47584,22.75093 0,0.78067 3.56877,13.3829 5.57621,18.28996 "
          "2.00743,4.90706 9.03346,15.94796 10.48327,17.50929 1.44981,1.56134 "
          "8.02974,6.9145 9.92565,7.91822 1.89591,1.00372 5.24164,2.00743 "
          "7.13755,1.89591 1.89591,-0.11152 5.24163,-0.66914 7.58364,-1.56134 "
          "2.34201,-0.89219 6.69145,-3.56877 9.59108,-6.35688 2.89963,-2.7881 "
          "11.26394,-13.04832 12.4907,-16.05948 1.22677,-3.01115 "
          "6.24536,-12.0446 6.24536,-12.0446 z",
          "#f05c5a", "#f05c5a"));

  add_shape_to_group(
      group,
      create_path(
          scene,
          "m 112.61136,110.08785 c 7.88594,-2.20806 9.93629,-3.15438 "
          "14.1947,-2.68122 4.25842,0.47316 7.72823,0.47316 10.09402,2.05035 "
          "2.36578,1.57719 5.52016,2.99666 7.09735,4.88928 1.57719,1.89263 "
          "4.25842,5.20473 4.73157,7.09736 0.47316,1.89263 2.68123,5.8356 "
          "2.68123,8.83226 0,2.99666 -0.15772,8.98998 -2.99667,11.51349 "
          "-2.83894,2.5235 -3.31209,5.20472 -8.04366,6.15104 -4.73157,0.94631 "
          "-9.30542,1.41947 -12.14437,1.26175 -2.83894,-0.15772 "
          "-7.88594,-1.26175 -7.88594,-1.26175 v 0",
          "#000000", "#fed700"));
  add_shape_to_group(
      group, create_path(
                 scene,
                 "m 232.95094,113.39995 c -6.15104,4.73157 -5.8356,3.46982 "
                 "-8.98998,8.51683 -3.15438,5.047 -4.57385,8.3591 "
                 "-4.57385,11.35576 0,2.99666 -0.31544,6.93964 1.73491,9.30542 "
                 "2.05034,2.36579 4.88929,5.04701 8.83226,6.46648 "
                 "3.94297,1.41947 13.40611,0.47316 13.40611,0.47316",
                 "#000000", "#fed700"));

  add_shape_to_group(
      group,
      create_path(
          scene,
          "m 163.71747,270.55762 c 2.67658,-4.01487 4.23792,-7.91821 "
          "6.57993,-9.7026 2.34201,-1.78439 2.56505,-3.34573 4.90706,-3.6803 "
          "2.34201,-0.33457 3.34573,-0.66914 4.90706,0.22305 1.56134,0.89219 "
          "2.45354,1.78439 3.45725,2.67658 1.00372,0.89219 1.89591,1.22677 "
          "2.00744,0.55762 0.11152,-0.66914 -1.67286,-1.44981 1.00372,-2.89963 "
          "2.67658,-1.44981 1.89591,-2.23048 4.68401,-2.11896 2.7881,0.11153 "
          "3.12268,-0.55762 4.57249,1.00372 1.44982,1.56134 1.89591,1.56134 "
          "3.12268,3.2342 1.22676,1.67286 1.11524,2.67658 0.66914,3.2342 "
          "-0.44609,0.55762 -3.12267,5.13011 -6.0223,6.9145 -2.89963,1.78439 "
          "-4.90707,4.57249 -8.47584,5.24164 -3.56877,0.66914 -7.24907,2.00743 "
          "-9.59108,1.22676 -2.342,-0.78067 -9.2565,-2.89963 -9.92565,-3.79182 "
          "-0.66914,-0.89219 -1.89591,-2.11896 -1.89591,-2.11896 z",
          "none", "#f4eff3"));
  add_shape_to_group(
      group,
      create_path(scene,
                  "m 155.24164,260.07435 c 2.67658,-10.1487 2.56505,-10.81784 "
                  "2.11895,-13.3829 -0.44609,-2.56506 0.11153,-4.46097 "
                  "-2.45353,-6.0223 -2.56505,-1.56134 -4.57249,-2.56506 "
                  "-6.35687,-2.45354 -1.78439,0.11153 -3.12268,0.66915 "
                  "-3.12268,0.66915 0,0 -1.78439,0 3.01115,9.14498 "
                  "4.79554,9.14498 6.80298,12.04461 6.80298,12.04461 z",
                  "none", "#f4eff3"));
  add_shape_to_group(
      group,
      create_path(
          scene,
          "m 157.13755,125.79926 c 1.11524,6.24535 -0.11153,4.23791 "
          "2.67658,9.36803 2.7881,5.13011 3.68029,5.13011 5.01858,5.5762 "
          "1.33829,0.4461 3.01116,1.22677 4.23792,0.66915 1.22677,-0.55762 "
          "2.56506,-1.67286 3.01115,-2.34201 0.4461,-0.66914 0.33458,-1.56134 "
          "1.11525,-2.00743 0.78067,-0.4461 1.00371,-0.22305 1.78438,0.55762 "
          "0.78067,0.78067 2.11896,2.11896 3.2342,2.67658 1.11525,0.55762 "
          "1.67287,1.11524 4.57249,1.44981 2.89963,0.33457 6.02231,1.11524 "
          "7.91822,0.22305 1.89591,-0.89219 4.46097,-0.89219 7.02602,-2.67658 "
          "2.56506,-1.78439 6.24536,-5.01859 7.80669,-6.9145 1.56134,-1.89591 "
          "2.89963,-2.89963 3.79183,-4.01487 0.89219,-1.11524 0.55762,-2.11896 "
          "-0.55763,-2.45353 -1.11524,-0.33457 -9.7026,-2.23048 "
          "-13.94052,-2.7881 -4.23791,-0.55762 -11.37546,-1.56134 "
          "-15.39033,-1.22677 -4.01487,0.33457 -10.26022,1.00372 "
          "-14.16357,1.89591 -3.90335,0.8922 -8.14126,2.00744 -8.14126,2.00744 "
          "z",
          "none", "#f4eff3"));

  return group;
}

shape_t *create_path(scene_t *sc, char *d, char *stroke, char *fill) {
  shape_t *s = create_base_shape(sc, PATH, stroke, fill);
  if (!s)
    return NULL;
  s->data.path.d = strdup(d);
  return s;
}
