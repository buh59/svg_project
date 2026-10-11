#include "../include/types.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  char input[256];

  while (true) {
    printf("svg> ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    if (strcmp(input, "quit") == 0) {
      break;
    } else if (strcmp(input, "help") == 0) {
      printf("[AFFICHE UN MESSAGE D'AIDE avec la fonction print_help()]");
    } else if (strcmp(input, "") == 0) {
      printf("empty\n");
    } else
      printf("unknown command\n");
  }
}
