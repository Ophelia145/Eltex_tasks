#include <fcntl.h> //работа с файловым дескриптором
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#define FIFOSNAME "/tmp/fifo"

void pipe_between_2_procs ();

void bash_with_pipes ();