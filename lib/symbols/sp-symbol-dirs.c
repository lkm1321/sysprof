/* sp-symbol-dirs.c
 *
 * Copyright (C) 2017 Christian Hergert <chergert@redhat.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "symbols/sp-symbol-dirs.h"

static GPtrArray *sp_symbol_dirs;

G_LOCK_DEFINE (sp_symbol_dirs);

static GPtrArray *
sp_get_symbol_dirs_locked (void)
{
  if (sp_symbol_dirs == NULL)
    {
      sp_symbol_dirs = g_ptr_array_new ();
      g_ptr_array_add (sp_symbol_dirs, g_strdup ("/usr/lib/debug"));
    }

  return sp_symbol_dirs;
}

void
sp_symbol_dirs_add (const gchar *path)
{
  GPtrArray *ar;

  G_LOCK (sp_symbol_dirs);

  ar = sp_get_symbol_dirs_locked ();

  for (guint i = 0; i < ar->len; i++)
    {
      const gchar *ele = g_ptr_array_index (ar, i);

      if (g_strcmp0 (path, ele) == 0)
        goto skip;
    }

  g_ptr_array_add (ar, g_strdup (path));

skip:
  G_UNLOCK (sp_symbol_dirs);
}

void
sp_symbol_dirs_remove (const gchar *path)
{
  GPtrArray *ar;

  G_LOCK (sp_symbol_dirs);

  ar = sp_get_symbol_dirs_locked ();

  for (guint i = 0; i < ar->len; i++)
    {
      const gchar *ele = g_ptr_array_index (ar, i);

      if (g_strcmp0 (path, ele) == 0)
        {
          g_ptr_array_remove_index (ar, i);
          break;
        }
    }

  G_UNLOCK (sp_symbol_dirs);
}

/**
 * sp_symbol_dirs_get_paths:
 * @dir: the directory containing the library
 * @name: the name of the file in @dir
 *
 * This function will build an array of files to look at to resolve the
 * debug symbols for the file at path "dir/name".
 *
 * Returns: (transfer full): A #GStrv of possible paths.
 */
gchar **
sp_symbol_dirs_get_paths (const gchar *dir,
                          const gchar *name)
{
  GPtrArray *ret = g_ptr_array_new ();
  GPtrArray *ar;

  g_ptr_array_add (ret, g_build_filename (dir, name, NULL));

  G_LOCK (sp_symbol_dirs);

  ar = sp_get_symbol_dirs_locked ();

  for (guint i = 0; i < ar->len; i++)
    {
      const gchar *ele = g_ptr_array_index (ar, i);

      g_ptr_array_add (ret, g_build_filename (ele, name, NULL));
      g_ptr_array_add (ret, g_build_filename (ele, dir, name, NULL));
    }

  g_ptr_array_add (ret, g_build_filename (dir, ".debug", name, NULL));
  g_ptr_array_add (ret, g_build_filename (DEBUGDIR, dir, name, NULL));

  G_UNLOCK (sp_symbol_dirs);

  g_ptr_array_add (ret, NULL);

  return (gchar **)g_ptr_array_free (ret, FALSE);
}
