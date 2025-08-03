#define _XOPEN_SOURCE_EXTENDED 1
#include <dirent.h> //for dir
#include <locale.h>
#include <ncurses.h>
#include <ncursesw/ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>
#define COLOR_PINK 12
#define COLOR_DARK_BLUE 13
#define maxN 256
#define WIN_INIT()                                                            \
  {                                                                           \
    initscr ();                                                               \
    noecho ();                                                                \
    curs_set (0);                                                             \
    keypad (stdscr, TRUE);                                                    \
    cbreak ();                                                                \
    nodelay (stdscr, 1);                                                      \
    start_color ();                                                           \
    use_default_colors ();                                                    \
  }

typedef struct panel
{
  WINDOW *win;
  wchar_t path_to_here[maxN];
  bool is_dir[maxN];
  wchar_t **files_folders;
  int quantity_of_files;
  int selected_ind;
  bool is_active;

  off_t sizes[maxN]; // файлов
  wchar_t mod_times[maxN][20];

} panel;

typedef struct file_inf
{
  wchar_t name[maxN];
  off_t size;
  time_t mtime;
  mode_t permissions;
  bool is_dir;
} file_inf;

void init_panel (panel *p, const wchar_t *initial_path, WINDOW *win);
void draw_panel (panel *p);
void open_selected (panel *p);
void reload_panel (panel *p);
void utf8_to_wstr (const char *utf8, wchar_t *wstr, size_t max_len);
void format_size (off_t size, wchar_t *buffer, size_t buf_size);
void handle_resize (int sig);