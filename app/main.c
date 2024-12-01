#include <stdio.h>
#include <string.h>

int main() {
  while (1) {
    printf("$ ");
    fflush(stdout);

    // Wait for user input
    char input[100];
    fgets(input, 100, stdin);

    printf("%.*s: command not found\n", (int)(strlen(input) - 1), input);
  }
  return 0;
}
