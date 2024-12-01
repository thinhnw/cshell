#include <stdio.h>
#include <string.h>

int main() {
  // Uncomment this block to pass the first stage
  printf("$ ");
  fflush(stdout);

  // Wait for user input
  char input[100];
  fgets(input, 100, stdin);

  printf("%.*s: command not found\n", (int)(strlen(input) - 1), input);
  return 0;
}
