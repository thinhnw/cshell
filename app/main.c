#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define PATH_MAX 4096

int which(const char *cmd_name, char **full_path) {
  char *path = getenv("PATH");
  if (!path) return 1;

  char *path_copy = strdup(path);

  char *dir = strtok(path_copy, ":");
  while (dir) {
    static char buffer[PATH_MAX];
    snprintf(buffer, sizeof(buffer), "%s/%s", dir, cmd_name);
    if (access(buffer, X_OK) == 0) {
      *full_path = strdup(buffer);
      free(path_copy);
      return 0;
    }
    dir = strtok(NULL, ":");
  }
  return 1;
}

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

    if (PATH != NULL) {
      char *full_path;
      if (which(command, &full_path) == 0) {
        puts(full_path);
        puts(command);
        char *args = input + strlen(command) + 1;
        args[strlen(args) - 1] = '\0';
        puts(args);
        pid_t pid;
        pid = fork();
        if (pid < 0) {
          perror("fork");
          continue;
        }
        if (pid == 0) {
          // run command in child process
          if (execl(full_path, command, args, NULL) == -1)
            perror("execl failed");
        }
        // wait for child process to finish
        int pid_status;
        if (waitpid(pid, &pid_status, 0) == -1) perror("waitpid");
        free(full_path);
        continue;
      }
    }

    printf("%s: command not found\n", command);
  }
  return 0;
}
