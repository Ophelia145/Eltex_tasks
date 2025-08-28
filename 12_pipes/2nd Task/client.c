#include "../main.h"

int main() {
  int fd = open(FIFOSNAME, O_RDONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  char buff[128];
  ssize_t n_bytes_read = read(fd, buff, sizeof(buff) - 1);

  if (n_bytes_read == -1) {
    perror("read");
    close(fd);
    exit(EXIT_FAILURE);
  }

  buff[n_bytes_read] = '\0';

  printf("we got : %s\n", buff);
  close(fd);

  if (unlink(FIFOSNAME) == -1) {
    perror("unlink");
    exit(EXIT_FAILURE);
  }
  return 0;
}