#include "main.h"
void
init_panel (panel *p, const wchar_t *initial_path, WINDOW *win)
{
  p->win = win;
  wcsncpy (p->path_to_here, initial_path, maxN);
  p->selected_ind = 0;
  p->quantity_of_files = 0;

  p->files_folders = malloc (maxN * sizeof (wchar_t *));
  for (int i = 0; i < maxN; i++)
    {
      p->files_folders[i] = malloc (maxN * sizeof (wchar_t));
    }

  reload_panel (p);
}
