#include "../main.h"

#define MAX_INPUT_LEN 1024  //макс длина вход команды
#define MAX_ARGS 64         //макс кол-во
#define MAX_PIPES 10

void bash_with_pipes() {
  char inp[MAX_INPUT_LEN];  //буфер для ввода команды
  char *args[MAX_ARGS];     // массив аргов
  while (1) {
    printf("$ ");
    fflush(stdout);

    if (!fgets(inp, MAX_INPUT_LEN, stdin)) break;
    inp[strcspn(inp, "\n")] = '\0';

    if (strcmp(inp, "exit") == 0) break;
    int i = 0;
    //парсим инпут на аргументф
    char *token = strtok(inp, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
      args[i++] = token;
      token = strtok(NULL, " ");
    }
    args[i] = NULL;  //сигнал уонца массива

    int pipe_indices[MAX_PIPES];
    int num_pipes = 0;

    for (int j = 0; args[j] != NULL && num_pipes < MAX_PIPES; j++) {
      if (strcmp(args[j], "|") == 0) {
        pipe_indices[num_pipes++] = j;
      }
    }
    //команды по трубам
    if (num_pipes > 0) {
      char *commands[MAX_PIPES + 1][MAX_ARGS];
      int cmd_count = num_pipes + 1;

      // 1ая команда
      int cmd_start = 0;
      for (int pipe_idx = 0; pipe_idx < num_pipes; pipe_idx++) {
        int cmd_end = pipe_indices[pipe_idx];
        int arg_count = 0;

        for (int j = cmd_start; j < cmd_end; j++) {
          commands[pipe_idx][arg_count++] = args[j];
        }
        commands[pipe_idx][arg_count] = NULL;
        cmd_start = cmd_end + 1;
      }

      // последняя команда
      int arg_count = 0;
      for (int j = cmd_start; args[j] != NULL; j++) {
        commands[num_pipes][arg_count++] = args[j];
      }
      commands[num_pipes][arg_count] = NULL;

      // создаем каналы
      int pipefds[MAX_PIPES][2];
      for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipefds[i]) == -1) {
          perror("pipe");
          continue;
        }
      }
      // исполним команды
      pid_t pids[cmd_count];
      for (int i = 0; i < cmd_count; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
          if (i > 0) {
            // перенаправим stdin из предыдущей трубы
            dup2(pipefds[i - 1][0], STDIN_FILENO);
          }
          if (i < num_pipes) {
            dup2(pipefds[i][1], STDOUT_FILENO);
          }
          for (int j = 0; j < num_pipes; j++) {
            close(pipefds[j][0]);
            close(pipefds[j][1]);
          }
          execvp(commands[i][0], commands[i]);
          perror("execvp");
          exit(EXIT_FAILURE);
        }
      }
      // в родителе закрываем трубы и ждем завершения дочерних
      for (int i = 0; i < num_pipes; i++) {
        close(pipefds[i][0]);
        close(pipefds[i][1]);
      }
      for (int i = 0; i < cmd_count; i++) {
        waitpid(pids[i], NULL, 0);
      }
      continue;
    }
    if (strcmp(args[0], "cd") == 0) {
      if (args[1] == NULL) {
        fprintf(stderr, "cd: missing argument\n");
      } else {
        if (chdir(args[1]) != 0) {  // Меняем директорию
          perror("cd");
        }
      }
    } else {
      pid_t pid = fork();
      if (pid == -1) {
        perror("fork");
        continue;
      }

      if (pid == 0) {
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
      } else {
        wait(NULL);
      }
    }
  }
}

// if вызвать cd через execvp, он изменит директорию дочернего процесса,
// который сразу завершится и родительский процесс
// останется в той же папке