#include "main.h"

void
reload_panel (panel *p)
{
  p->quantity_of_files = 0;

  char path_utf8[maxN];
  wcstombs (path_utf8, p->path_to_here, maxN);

  DIR *dir = opendir (path_utf8);
  if (!dir)
    return;

  struct dirent *entry;
  while ((entry = readdir (dir)) != NULL && p->quantity_of_files < maxN)
    {
      if (entry->d_name[0] == '.' && strcmp (entry->d_name, ".") != 0
          && strcmp (entry->d_name, "..") != 0)
        {
          continue;
        }

      p->files_folders[p->quantity_of_files]
          = malloc (maxN * sizeof (wchar_t));
      utf8_to_wstr (entry->d_name, p->files_folders[p->quantity_of_files],
                    maxN);

      char full_path_utf8[2 * maxN];
      snprintf (full_path_utf8, sizeof (full_path_utf8), "%s/%s", path_utf8,
                entry->d_name);

      struct stat st;
      if (stat (full_path_utf8, &st) == 0)
        {
          p->is_dir[p->quantity_of_files] = S_ISDIR (st.st_mode);
          p->sizes[p->quantity_of_files] = st.st_size;

          struct tm *tm_info = localtime (&st.st_mtime);
          char time_utf8[20];
          strftime (time_utf8, sizeof (time_utf8), "%Y-%m-%d %H:%M", tm_info);
          utf8_to_wstr (time_utf8, p->mod_times[p->quantity_of_files], 20);
        }
      else
        {
          p->is_dir[p->quantity_of_files] = false;
          p->sizes[p->quantity_of_files] = 0;
          wcscpy (p->mod_times[p->quantity_of_files], L"???");
        }

      p->quantity_of_files++;
    }
  closedir (dir);
}