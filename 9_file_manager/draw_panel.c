#include "main.h"

void
draw_panel (panel *p)
{
  werase (p->win);
  box (p->win, 0, 0);

  int win_y, win_x;
  getmaxyx (p->win, win_y, win_x);
  int name_width = win_x * 0.5;
  int size_width = win_x * 0.2;
  int time_width = win_x - name_width - size_width - 3;

  mvwaddwstr (p->win, 0, 2, p->path_to_here);

  mvwaddwstr (p->win, 1, 1, L"Имя");
  mvwaddwstr (p->win, 1, name_width + 2, L"Размер");
  mvwaddwstr (p->win, 1, name_width + size_width + 3, L"Изменен");

  int start_index = 0;
  int visible_items = win_y - 3;

  if (p->selected_ind >= start_index + visible_items)
    {
      start_index = p->selected_ind - visible_items + 1;
    }

  int row = 2;
  for (int i = start_index; i < p->quantity_of_files && row < win_y; i++)
    {
      if (i == p->selected_ind && p->is_active)
        {
          wattron (p->win, A_REVERSE);
        }

      if (p->is_dir[i])
        {
          wattron (p->win, COLOR_PAIR (2));
        }
      else
        {
          wattron (p->win, COLOR_PAIR (3));
        }

      mvwaddwstr (p->win, row, 1, p->files_folders[i]);

      wchar_t size_str[20];
      if (p->is_dir[i])
        {
          wcscpy (size_str, L"<DIR>");
        }
      else
        {
          format_size (p->sizes[i], size_str, 20);
        }
      mvwaddwstr (p->win, row, name_width + 2, size_str);

      mvwaddwstr (p->win, row, name_width + size_width + 3, p->mod_times[i]);

      if (p->is_dir[i])
        {
          wattroff (p->win, COLOR_PAIR (2));
        }
      else
        {
          wattroff (p->win, COLOR_PAIR (3));
        }

      if (i == p->selected_ind && p->is_active)
        {
          wattroff (p->win, A_REVERSE);
        }

      row++;
    }

  wrefresh (p->win);
}