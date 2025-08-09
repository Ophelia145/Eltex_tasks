#include "main.h"
#define MAX_INPUT_LEN 1024
#define MAX_ARGS 64
void
abash ()
{
  char inp[MAX_INPUT_LEN];
  char *args[MAX_ARGS];
  while (1)
    {

      printf ("$ ");
      fflush (stdout);

      if (!fgets (inp, MAX_INPUT_LEN, stdin))
        break;
      inp[strcspn (inp, "\n")] = '\0';

      if (strcmp (inp, "exit") == 0)
        break;
      int i = 0;
      char *token = strtok (inp, " ");
      while (token != NULL && i < MAX_ARGS - 1)
        {
          args[i++] = token;
          token = strtok (NULL, " ");
        }
      args[i] = NULL; //сигнал уонца массива

      int is_pipeline = 0;
      int pipe_index = -1;
      for (int j = 0; args[j] != NULL; j++)
        {
          if (strcmp (args[j], "|") == 0)
            {
              is_pipeline = 1;
              pipe_index = j;
              break;
            }
        }

      if (is_pipeline)
        {
          char *cmd1[MAX_ARGS];
          char *cmd2[MAX_ARGS];

          // 1ая команда
          for (i = 0; i < pipe_index; i++)
            {
              cmd1[i] = args[i];
            }
          cmd1[pipe_index] = NULL;

          // 2nd команда -после |
          int k = 0;
          for (i = pipe_index + 1; args[i] != NULL; i++)
            {
              cmd2[k++] = args[i];
            }
          cmd2[k] = NULL;

          // create pipe
          int pipefd[2];
          if (pipe (pipefd))
            {
              perror ("pipe");
              continue;
            }

          pid_t pid1 = fork ();
          if (pid1 == 0)
            {
              close (pipefd[0]); // закрываем неиспользуемый конец чтения
              dup2 (pipefd[1], STDOUT_FILENO); // перенаправляем вывод в pipe
              close (pipefd[1]);

              execvp (cmd1[0], cmd1);
              perror ("execvp (cmd1)");
              exit (EXIT_FAILURE);
            }

          // команда справа от |
          pid_t pid2 = fork ();
          if (pid2 == 0)
            {
              close (pipefd[1]); // закрываем неиспользуемый конец записи
              dup2 (pipefd[0], STDIN_FILENO); // перенаправляем ввод из pipe
              close (pipefd[0]);

              execvp (cmd2[0], cmd2);
              perror ("execvp (cmd2)");
              exit (EXIT_FAILURE);
            }

          // род процесс
          close (pipefd[0]);
          close (pipefd[1]);
          waitpid (pid1, NULL, 0);
          waitpid (pid2, NULL, 0);
        }
      //////

      else if (strcmp (args[0], "cd") == 0)
        {
          if (args[1] == NULL)
            {
              fprintf (stderr, "cd: missing argument\n");
            }
          else
            {
              if (chdir (args[1]) != 0)
                { // Меняем директорию
                  perror ("cd");
                }
            }
          continue; // Пропускаем fork/execvp
        }
      //////
      else
        {
          pid_t pid = fork ();
          if (pid == -1)
            {
              perror ("fork");
              continue;
            }

          if (pid == 0)
            {
              execvp (args[0], args);
              perror ("execvp");
              exit (EXIT_FAILURE);
            }
          else
            {
              wait (NULL);
            }
        }
    }
}

// if вызвать cd через execvp, он изменит директорию дочернего процесса,
// который сразу завершится и родительский процесс
// останется в той же папке