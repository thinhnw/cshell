#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
  while (1) {
    printf("$ ");
    fflush(stdout);

    // Wait for user input
    char input[100];
    fgets(input, 100, stdin);

    char *command = strtok(input, " \n");
    if (command == NULL) {
      continue; 
    } else if (strcmp(command, "exit") == 0) {
      exit(0);
    } else if (strcmp(command, "echo") == 0) {
      printf("%s", input + strlen(command) + 1);
    } else {
      printf("%s: command not found\n", command);
    }
  }
  return 0;
}
