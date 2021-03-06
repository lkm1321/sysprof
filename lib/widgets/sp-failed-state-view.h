/* sp-failed-state-view.h
 *
 * Copyright (C) 2016 Christian Hergert <chergert@redhat.com>
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

#ifndef SP_FAILED_STATE_VIEW_H
#define SP_FAILED_STATE_VIEW_H

#include <gtk/gtk.h>

#include "profiler/sp-profiler.h"

G_BEGIN_DECLS

#define SP_TYPE_FAILED_STATE_VIEW (sp_failed_state_view_get_type())

G_DECLARE_DERIVABLE_TYPE (SpFailedStateView, sp_failed_state_view, SP, FAILED_STATE_VIEW, GtkBin)

struct _SpFailedStateViewClass
{
  GtkBinClass parent;

  gpointer padding[4];
};

GtkWidget *sp_failed_state_view_new          (void);
void       sp_failed_state_view_set_profiler (SpFailedStateView *self,
                                              SpProfiler        *profiler);

G_END_DECLS

#endif /* SP_FAILED_STATE_VIEW_H */
