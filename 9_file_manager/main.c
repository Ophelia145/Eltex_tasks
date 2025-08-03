#include "main.h"

volatile sig_atomic_t resized = 0;

void
handle_resize (int sig)
{
  resized = 1;
}
void
utf8_to_wstr (const char *utf8, wchar_t *wstr, size_t max_len)
{
  size_t len = mbstowcs (wstr, utf8, max_len);
  if (len == (size_t)-1)
    {
      wcscpy (wstr, L"???");
    }
}

int
main ()
{
  setlocale (LC_ALL, "ru_RU.UTF-8");
  WIN_INIT ();
  signal (SIGWINCH, handle_resize);

  init_color (COLOR_DARK_BLUE, 10, 100, 355);
  init_color (COLOR_PINK, 900, 800, 750);
  init_color (COLOR_MAGENTA, 800, 200, 600);
  init_color (COLOR_CYAN, 400, 700, 900);
  init_pair (1, COLOR_PINK, COLOR_DARK_BLUE);
  init_pair (2, COLOR_MAGENTA, COLOR_DARK_BLUE);
  init_pair (3, COLOR_CYAN, COLOR_DARK_BLUE);

  int y, x;
  getmaxyx (stdscr, y, x);
  panel l_pan = { 0 }, r_pan = { 0 };
  l_pan.is_active = true;
  r_pan.is_active = false;

  l_pan.win = newwin (y, x / 2, 0, 0);
  r_pan.win = newwin (y, x / 2, 0, x / 2);

  //путь в широких символах
  wchar_t home_path[maxN];
  char *home_utf8 = getenv ("HOME");
  if (home_utf8)
    {
      utf8_to_wstr (home_utf8, home_path, maxN);
    }
  else
    {
      wcscpy (home_path, L"/");
    }

  init_panel (&l_pan, home_path, l_pan.win);
  init_panel (&r_pan, home_path, r_pan.win);

  wbkgd (l_pan.win, COLOR_PAIR (1));
  wbkgd (r_pan.win, COLOR_PAIR (1));

  draw_panel (&l_pan);
  draw_panel (&r_pan);
  refresh ();

  timeout (50); // неблокирующий ввод с таймаутом
  while (1)
    {
      if (resized)
        {
          resized = 0;
          int new_y, new_x;
          getmaxyx (stdscr, new_y, new_x);

          delwin (l_pan.win);
          delwin (r_pan.win);

          l_pan.win = newwin (new_y, new_x / 2, 0, 0);
          r_pan.win = newwin (new_y, new_x / 2, 0, new_x / 2);

          wbkgd (l_pan.win, COLOR_PAIR (1));
          wbkgd (r_pan.win, COLOR_PAIR (1));

          reload_panel (&l_pan);
          reload_panel (&r_pan);
          redrawwin (stdscr);
          refresh ();
        }

      int ch = getch ();
      if (ch == 'q' || ch == 'Q')
        break;

      panel *active = l_pan.is_active ? &l_pan : &r_pan;

      switch (ch)
        {
        case KEY_UP:
          if (active->selected_ind > 0)
            active->selected_ind--;
          break;
        case KEY_DOWN:
          if (active->selected_ind < active->quantity_of_files - 1)
            active->selected_ind++;
          break;
        case '\t':
          l_pan.is_active = !l_pan.is_active;
          r_pan.is_active = !r_pan.is_active;
          break;
        case '\n':
        case KEY_ENTER:
          open_selected (active);
          break;
        case ERR: // таймаут, ничего не делать
          break;
        }

      draw_panel (&l_pan);
      draw_panel (&r_pan);
    }

  for (int i = 0; i < maxN; i++)
    {
      free (l_pan.files_folders[i]);
      free (r_pan.files_folders[i]);
    }
  free (l_pan.files_folders);
  free (r_pan.files_folders);

  delwin (l_pan.win);
  delwin (r_pan.win);
  endwin ();
  return 0;
}