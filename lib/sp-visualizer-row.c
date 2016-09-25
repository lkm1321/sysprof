/* sp-visualizer-row.c
 *
 * Copyright (C) 2016 Christian Hergert <christian@hergert.me>
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

#include "sp-visualizer-row.h"

G_DEFINE_ABSTRACT_TYPE (SpVisualizerRow, sp_visualizer_row, GTK_TYPE_LIST_BOX_ROW)

static void
sp_visualizer_row_class_init (SpVisualizerRowClass *klass)
{
}

static void
sp_visualizer_row_init (SpVisualizerRow *self)
{
}

void
sp_visualizer_row_set_reader (SpVisualizerRow *self,
                              SpCaptureReader *reader)
{
  g_return_if_fail (SP_IS_VISUALIZER_ROW (self));

  if (SP_VISUALIZER_ROW_GET_CLASS (self)->set_reader)
    SP_VISUALIZER_ROW_GET_CLASS (self)->set_reader (self, reader);
}