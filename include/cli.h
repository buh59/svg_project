#ifndef CLI_H
#define CLI_H

#include "scene.h"
#include "types.h"
#include <stdbool.h>

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

/**
 * @brief Displays a simulated loading animation in the console.
 * @param message The message text to display during loading.
 * @param color The ANSI color code to use for rendering.
 */
void animate_loading(const char *message, const char *color);

/**
 * @brief Pauses the program execution until the user presses Enter.
 */
void press_enter();

/**
 * @brief Safely prompts the user for an integer input.
 * Loops continuously until a valid integer within predefined bounds is
 * provided.
 * @param title The main prompt question or title.
 * @param hint Additional hint message to guide the user.
 * @param color The ANSI color code for the interface element.
 * @return The validated integer input by the user.
 */
int ask_int(const char *title, const char *hint, const char *color);

/**
 * @brief Safely prompts the user for a string input.
 * Loops continuously until a non-empty string is entered.
 * @param title The main prompt question or title.
 * @param hint Additional hint message to guide the user.
 * @param buffer The character array buffer where the input will be stored.
 * @param size The maximum size of the buffer to prevent buffer overflow.
 * @param color The ANSI color code for the interface element.
 */
void ask_string(const char *title, const char *hint, char *buffer, int size,
                const char *color);

/**
 * @brief Renders the main application dashboard and the current scene tree
 * structure.
 * @param scene Pointer to the current scene to display.
 */
void print_dashboard(scene_t *scene);

/**
 * @brief Displays the available menu options to the user.
 */
void print_options(void);

/**
 * @brief Handles the user interface and logic for creating a new shape.
 * @param scene Pointer to the current scene where the shape will be added.
 */
void page_create(scene_t *scene);

/**
 * @brief Handles the user interface and logic for modifying an existing shape.
 * @param scene Pointer to the current scene containing the target shape.
 */
void page_modify(scene_t *scene);

#endif
