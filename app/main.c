#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  const char *builtins[] = {"type", "exit", "echo"};
  size_t num_builtins = sizeof(builtins) / sizeof(builtins[0]);

  while (1) {
    printf("$ ");
    fflush(stdout);

    // Wait for user input
    char input[100];
    fgets(input, 100, stdin);

    char *command = strtok(input, " \n");

    if (command == NULL) continue;

    if (strcmp(command, "type") == 0) {
      command = strtok(NULL, " \n");
      if (command == NULL) continue;
      int found = 0;
      for (size_t i = 0; i < num_builtins; i++)
        if (strcmp(command, builtins[i]) == 0) {
          printf("%s is a shell builtin\n", command);
          found = 1;
          break;
        }
      if (!found) printf("%s: not found\n", command);
      continue;
    }

    if (strcmp(command, "exit") == 0) exit(0);

    if (strcmp(command, "echo") == 0) {
      printf("%s", input + strlen(command) + 1);
      continue;
    }

    printf("%s: command not found\n", command);
  }
  return 0;
}
