#ifndef SHAPE_H
#define SHAPE_H

#include "types.h"

// =========================================================================
// GESTION DE LA SCÈNE ET DES FORMES DE BASE
// =========================================================================

/**
 * @brief Alloue et initialise une nouvelle scène SVG.
 * @param width Largeur du viewport.
 * @param height Hauteur du viewport.
 * @return Pointeur vers la scène créée.
 */
scene_t *create_scene(int width, int height);

/**
 * @brief Libère intégralement la mémoire occupée par la scène et ses enfants.
 * @param scene Pointeur vers la scène à détruire.
 */
void free_scene(scene_t *scene);

/**
 * @brief Ajoute une forme à la racine de la scène.
 * @param scene La scène cible.
 * @param shape La forme à ajouter.
 */
void add_shape_to_scene(scene_t *scene, shape_t *shape);

/**
 * @brief Cherche récursivement une forme dans l'arbre par son ID.
 * @param current Nœud de départ (généralement scene->head).
 * @param target_id L'identifiant à rechercher.
 * @return Pointeur vers la forme, ou NULL si introuvable.
 */
shape_t *find_shape_by_id(shape_t *current, int target_id);

/**
 * @brief Isole, détruit et libère la mémoire d'une forme (et de ses enfants si
 * groupe).
 * @param scene La scène courante.
 * @param target_id L'ID de la forme à supprimer.
 */
void delete_shape(scene_t *scene, int target_id);

/**
 * @brief Libère la mémoire d'une forme spécifique de manière récursive.
 * @param shape La forme à détruire.
 */
void free_shape(shape_t *shape);

// =========================================================================
// CRÉATION DES GÉOMÉTRIES
// =========================================================================

shape_t *create_rectangle(scene_t *scene, int x, int y, int width, int height,
                          char *color_stroke, char *color_fill);
shape_t *create_circle(scene_t *scene, int cx, int cy, int r,
                       char *color_stroke, char *color_fill);
shape_t *create_ellipse(scene_t *scene, int cx, int cy, int rx, int ry,
                        char *color_stroke, char *color_fill);
shape_t *create_line(scene_t *scene, int x1, int y1, int x2, int y2,
                     char *color_stroke, char *color_fill);

/**
 * @brief Crée une structure générique pour Polyline ou Polygon.
 * @param scene La scène courante.
 * @param type Le type exact (POLYLINE ou POLYGON).
 * @param color_stroke Couleur du trait.
 * @param color_fill Couleur de remplissage.
 * @return Pointeur vers la forme créée.
 */
shape_t *create_point_list_shape(scene_t *scene, shape_type_t type,
                                 char *color_stroke, char *color_fill);

/**
 * @brief Ajoute un point de coordonnée (x, y) à une liste de points.
 * @param shape La forme cible (doit être POLYLINE ou POLYGON).
 * @param x Coordonnée X.
 * @param y Coordonnée Y.
 * @return true si l'ajout a réussi, false sinon.
 */
bool add_point(shape_t *shape, int x, int y);

// =========================================================================
// GESTION DES GROUPES (ARBRE N-AIRE)
// =========================================================================

/**
 * @brief Isole une forme de sa liste courante sans la détruire en mémoire.
 * @param shape La forme à détacher.
 * @param scene La scène de référence.
 */
void detach_shape(shape_t *shape, scene_t *scene);

/**
 * @brief Crée un nouveau groupe vide.
 * @param scene La scène courante.
 * @return Pointeur vers le groupe créé.
 */
shape_t *create_group(scene_t *scene);

/**
 * @brief Ajoute une forme existante à l'intérieur d'un groupe.
 * @param group Le groupe cible.
 * @param child La forme enfant à insérer.
 */
void add_shape_to_group(shape_t *group, shape_t *child);

/**
 * @brief Crée un groupe et y insère directement une liste d'IDs existants.
 * @param scene La scène courante.
 * @param ids Tableau des IDs à regrouper.
 * @param count Nombre d'IDs dans le tableau.
 * @return Pointeur vers le groupe créé.
 */
shape_t *create_group_with(scene_t *scene, int *ids, int count);

/**
 * @brief Retire une forme de son groupe pour la replacer à la racine de la
 * scène.
 * @param scene La scène courante.
 * @param target_id L'ID de la forme à extraire.
 */
void remove_from_group(scene_t *scene, int target_id);

/**
 * @brief Vérifie si une forme est l'ancêtre d'une autre (Prévention de boucle
 * infinie).
 * @param potential_ancestor Le parent potentiel à vérifier.
 * @param shape L'enfant testé.
 * @return true si potential_ancestor est dans la chaîne des parents de shape.
 */
bool is_ancestor(shape_t *potential_ancestor, shape_t *shape);

#endif
