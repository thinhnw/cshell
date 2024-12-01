#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  const char *builtins[] = {"type", "exit", "echo"};
  size_t num_builtins = sizeof(builtins) / sizeof(builtins[0]);

  const char *PATH = getenv("PATH");

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

      if (found || PATH == NULL) continue;

      char *path_copy = strdup(PATH);
      char *dir = strtok(path_copy, ":");

      while (dir != NULL) {
        DIR *dp = opendir(dir);
        if (dp == NULL) {
          perror(dir);
          dir = strtok(NULL, ":");
          continue;
        }

        struct dirent *entry;
        while ((entry = readdir(dp)) != NULL) {
          if (strcmp(entry->d_name, command) != 0) continue;
          char full_path[1024];
          snprintf(full_path, sizeof(full_path), "%s/%s", dir, entry->d_name);
          if (access(full_path, X_OK) == 0) {
            printf("%s is %s\n", command, full_path);
            found = 1;
            break;
          }
        }

        dir = strtok(NULL, ":");
      }

      free(path_copy);

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
