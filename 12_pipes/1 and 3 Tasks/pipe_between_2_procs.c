#include "../main.h"

void pipe_between_2_procs() {
  int fd[2];
  if (pipe(fd) == -1) {
    perror("pipe wasnt created");
    exit(EXIT_FAILURE);
  }
  pid_t pid = fork();
  if (pid == -1) {
    perror("process wasnt created");
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    close(fd[0]);
    const char *str = "Hi!";
    size_t len = strlen(str) + 1;
    ssize_t n_written = write(fd[1], str, len);
    if (n_written == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
    close(fd[1]);
    wait(NULL);
  } else {
    char buf[128];  //запас для модификаций
    close(fd[1]);
    ssize_t n_read = read(fd[0], buf, sizeof(buf) - 1);  //   для '\0'
    if (n_read == -1) {
      perror("read");
      _exit(EXIT_FAILURE);
    }
    buf[n_read] = '\0';
    printf("%s\n", buf);
    close(fd[0]);
    _exit(EXIT_SUCCESS);  //_ чтоб избежать попытки повторно очистить общий
                          //буфер (в родителе  exit)
  }
}
