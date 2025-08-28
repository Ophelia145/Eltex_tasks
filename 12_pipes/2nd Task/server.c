#include "../main.h"
int main() {
  if (mkfifo(FIFOSNAME, 0666) == -1) {
    perror("fifo has been created already");
    exit(1);
  }

  int fd = open(FIFOSNAME, O_WRONLY);
  if (fd == -1) {
    perror("open for write");
    exit(EXIT_FAILURE);
  }

  const char *msg = "Hi!";
  if (write(fd, msg, strlen(msg)) == -1) {
    perror("write :(");
    close(fd);
    exit(EXIT_FAILURE);
  }

  close(fd);
  return 0;
}
