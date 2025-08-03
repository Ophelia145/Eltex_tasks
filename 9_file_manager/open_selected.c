#include "main.h"

void
open_selected (panel *p)
{
  if (p->quantity_of_files == 0)
    return;

  char path_utf8[maxN];
  wcstombs (path_utf8, p->path_to_here, maxN);

  char name_utf8[maxN];
  wcstombs (name_utf8, p->files_folders[p->selected_ind], maxN);

  // Обработка ..
  if (strcmp (name_utf8, "..") == 0)
    {
      wchar_t *last_slash = wcsrchr (p->path_to_here, L'/');
      if (last_slash)
        {
          if (last_slash == p->path_to_here)
            {
              wcscpy (p->path_to_here, L"/");
            }
          else
            {
              *last_slash = L'\0';
            }
          p->selected_ind = 0;
          reload_panel (p);
        }
      return;
    }

  char full_path_utf8[2 * maxN];
  snprintf (full_path_utf8, sizeof (full_path_utf8), "%s/%s", path_utf8,
            name_utf8);

  struct stat st;
  if (stat (full_path_utf8, &st) != 0)
    return;

  if (S_ISDIR (st.st_mode))
    {
      wchar_t full_path_wstr[maxN];
      utf8_to_wstr (full_path_utf8, full_path_wstr, maxN);
      wcsncpy (p->path_to_here, full_path_wstr, maxN);
      p->selected_ind = 0;
      reload_panel (p);
    }
  else
    {
      char command[2 * maxN + 20];
      snprintf (command, sizeof (command), "xdg-open \"%s\" &",
                full_path_utf8);
      system (command);
    }
}
