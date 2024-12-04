#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define INPUT_MAX 100
#define PATH_MAX 4096
const char *builtins[] = {"type", "exit", "echo", "pwd"};
size_t num_builtins = sizeof(builtins) / sizeof(builtins[0]);

int find_command_in_path(const char *cmd_name, char **full_path) {
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
      return 1;
    }
    dir = strtok(NULL, ":");
  }
  free(path_copy);
  return 0;
}

void handle_type(const char *cmd) {
  if (cmd == NULL) return;

  // check if cmd is a builtin
  for (size_t i = 0; i < num_builtins; i++)
    if (strcmp(cmd, builtins[i]) == 0) {
      printf("%s is a shell builtin\n", cmd);
      return;
    }

  // check if cmd is in PATH
  const char *PATH = getenv("PATH");
  if (PATH == NULL) return;

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
      if (strcmp(entry->d_name, cmd) != 0) continue;
      char full_path[1024];
      snprintf(full_path, sizeof(full_path), "%s/%s", dir, entry->d_name);
      if (access(full_path, X_OK) == 0) {
        printf("%s is %s\n", cmd, full_path);
        free(path_copy);
        return;
      }
    }

    dir = strtok(NULL, ":");
  }

  free(path_copy);
  printf("%s: not found\n", cmd);
}

void exec_cmd(char *cmd) {
  char *cmd_name = strtok(cmd, " \n");

  if (cmd_name == NULL) return;

  if (strcmp(cmd_name, "type") == 0) {
    handle_type(strtok(NULL, " \n"));
    return;
  }

  if (strcmp(cmd_name, "exit") == 0) exit(0);

  if (strcmp(cmd_name, "echo") == 0) {
    printf("%s", cmd + strlen(cmd_name) + 1);
    return;
  }
  // handle external command
  const char *PATH = getenv("PATH");
  if (PATH == NULL) {
    printf("%s: command not found\n", cmd_name);
    return;
  }
  char *full_path;
  if (find_command_in_path(cmd_name, &full_path)) {
    char *args = cmd + strlen(cmd_name) + 1;
    args[strlen(args) - 1] = '\0';
    pid_t pid;
    pid = fork();
    if (pid < 0) {
      perror("fork");
      return;
    }
    // run command in child process
    if (pid == 0) {
      if (execl(full_path, cmd_name, args, NULL) == -1) perror("execl failed");
    }
    // wait for child process to finish
    int pid_status;
    if (waitpid(pid, &pid_status, 0) == -1) perror("waitpid");
    free(full_path);
    return;
  }

  printf("%s: command not found\n", cmd_name);
}

int main() {
  while (1) {
    printf("$ ");
    fflush(stdout);

    // Wait for user input
    char input[INPUT_MAX];
    fgets(input, INPUT_MAX, stdin);

    exec_cmd(input);
  }
  return 0;
}
