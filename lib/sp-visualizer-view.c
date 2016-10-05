/* sp-visualizer-view.c
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

#define G_LOG_DOMAIN "sp-visualizer-view"

#include <glib/gi18n.h>

#include "sp-visualizer-list.h"
#include "sp-visualizer-ticks.h"
#include "sp-visualizer-view.h"

#define NSEC_PER_SEC G_GINT64_CONSTANT(1000000000)

typedef struct
{
  SpCaptureReader   *reader;
  SpZoomManager     *zoom_manager;

  SpVisualizerList  *list;
  GtkAdjustment     *scroll_adjustment;
  SpVisualizerTicks *ticks;
} SpVisualizerViewPrivate;

enum {
  PROP_0,
  PROP_READER,
  PROP_ZOOM_MANAGER,
  N_PROPS
};

enum {
  VISUALIZER_ADDED,
  VISUALIZER_REMOVED,
  N_SIGNALS
};

static void buildable_iface_init (GtkBuildableIface *iface);

G_DEFINE_TYPE_EXTENDED (SpVisualizerView, sp_visualizer_view, GTK_TYPE_BIN, 0,
                        G_ADD_PRIVATE (SpVisualizerView)
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE, buildable_iface_init))

static GParamSpec *properties [N_PROPS];
static guint signals [N_SIGNALS];
static GtkBuildableIface *parent_buildable;

static void
sp_visualizer_view_row_added (SpVisualizerView *self,
                              GtkWidget        *widget,
                              SpVisualizerList *list)
{
  g_assert (SP_IS_VISUALIZER_VIEW (self));
  g_assert (GTK_IS_WIDGET (widget));
  g_assert (SP_IS_VISUALIZER_LIST (list));

  if (SP_IS_VISUALIZER_ROW (widget))
    g_signal_emit (self, signals [VISUALIZER_ADDED], 0, widget);
}

static void
sp_visualizer_view_row_removed (SpVisualizerView *self,
                                GtkWidget        *widget,
                                SpVisualizerList *list)
{
  g_assert (SP_IS_VISUALIZER_VIEW (self));
  g_assert (GTK_IS_WIDGET (widget));
  g_assert (SP_IS_VISUALIZER_LIST (list));

  if (SP_IS_VISUALIZER_ROW (widget))
    g_signal_emit (self, signals [VISUALIZER_REMOVED], 0, widget);
}

static void
sp_visualizer_view_finalize (GObject *object)
{
  SpVisualizerView *self = (SpVisualizerView *)object;
  SpVisualizerViewPrivate *priv = sp_visualizer_view_get_instance_private (self);

  g_clear_pointer (&priv->reader, sp_capture_reader_unref);
  g_clear_object (&priv->zoom_manager);

  G_OBJECT_CLASS (sp_visualizer_view_parent_class)->finalize (object);
}

static void
sp_visualizer_view_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  SpVisualizerView *self = (SpVisualizerView *)object;

  switch (prop_id)
    {
    case PROP_READER:
      g_value_set_boxed (value, sp_visualizer_view_get_reader (self));
      break;

    case PROP_ZOOM_MANAGER:
      g_value_set_object (value, sp_visualizer_view_get_zoom_manager (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
sp_visualizer_view_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  SpVisualizerView *self = (SpVisualizerView *)object;

  switch (prop_id)
    {
    case PROP_READER:
      sp_visualizer_view_set_reader (self, g_value_get_boxed (value));
      break;

    case PROP_ZOOM_MANAGER:
      sp_visualizer_view_set_zoom_manager (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
sp_visualizer_view_class_init (SpVisualizerViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = sp_visualizer_view_finalize;
  object_class->get_property = sp_visualizer_view_get_property;
  object_class->set_property = sp_visualizer_view_set_property;

  properties [PROP_READER] =
    g_param_spec_boxed ("reader",
                        "Reader",
                        "The reader for the visualizers",
                        SP_TYPE_CAPTURE_READER,
                        (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_ZOOM_MANAGER] =
    g_param_spec_object ("zoom-manager",
                         "Zoom Manager",
                         "The zoom manager for the view",
                         SP_TYPE_ZOOM_MANAGER,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  signals [VISUALIZER_ADDED] =
    g_signal_new ("visualizer-added",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (SpVisualizerViewClass, visualizer_added),
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1, SP_TYPE_VISUALIZER_ROW);

  signals [VISUALIZER_REMOVED] =
    g_signal_new ("visualizer-removed",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (SpVisualizerViewClass, visualizer_removed),
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1, SP_TYPE_VISUALIZER_ROW);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/sysprof/ui/sp-visualizer-view.ui");
  gtk_widget_class_bind_template_child_private (widget_class, SpVisualizerView, list);
  gtk_widget_class_bind_template_child_private (widget_class, SpVisualizerView, scroll_adjustment);
  gtk_widget_class_bind_template_child_private (widget_class, SpVisualizerView, ticks);

  gtk_widget_class_set_css_name (widget_class, "visualizers");
}

static void
sp_visualizer_view_init (SpVisualizerView *self)
{
  SpVisualizerViewPrivate *priv = sp_visualizer_view_get_instance_private (self);

  gtk_widget_init_template (GTK_WIDGET (self));

  g_signal_connect_object (priv->list,
                           "add",
                           G_CALLBACK (sp_visualizer_view_row_added),
                           self,
                           G_CONNECT_SWAPPED);

  g_signal_connect_object (priv->list,
                           "remove",
                           G_CALLBACK (sp_visualizer_view_row_removed),
                           self,
                           G_CONNECT_SWAPPED);
}

/**
 * sp_visualizer_view_get_reader:
 *
 * Returns: (transfer none): An #SpCaptureReader
 */
SpCaptureReader *
sp_visualizer_view_get_reader (SpVisualizerView *self)
{
  SpVisualizerViewPrivate *priv = sp_visualizer_view_get_instance_private (self);

  g_return_val_if_fail (SP_IS_VISUALIZER_VIEW (self), NULL);

  return priv->reader;
}

void
sp_visualizer_view_set_reader (SpVisualizerView *self,
                               SpCaptureReader  *reader)
{
  SpVisualizerViewPrivate *priv = sp_visualizer_view_get_instance_private (self);

  g_return_if_fail (SP_IS_VISUALIZER_VIEW (self));

  if (priv->reader != reader)
    {
      g_clear_pointer (&priv->reader, sp_capture_reader_unref);
      if (reader != NULL)
        priv->reader = sp_capture_reader_ref (reader);
      sp_visualizer_list_set_reader (priv->list, reader);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_READER]);
    }
}

static void
sp_visualizer_view_add_child (GtkBuildable *buildable,
                              GtkBuilder   *builder,
                              GObject      *child,
                              const gchar  *type)
{
  SpVisualizerView *self = (SpVisualizerView *)buildable;
  SpVisualizerViewPrivate *priv = sp_visualizer_view_get_instance_private (self);

  g_assert (SP_IS_VISUALIZER_VIEW (self));
  g_assert (GTK_IS_BUILDER (builder));
  g_assert (G_IS_OBJECT (child));

  if (g_strcmp0 (type, "visualizer") == 0 && GTK_IS_WIDGET (child))
    {
      gtk_container_add (GTK_CONTAINER (priv->list), GTK_WIDGET (child));
      return;
    }

  parent_buildable->add_child (buildable, builder, child, type);
}

static void
buildable_iface_init (GtkBuildableIface *iface)
{
  parent_buildable = g_type_interface_peek_parent (iface);
  iface->add_child = sp_visualizer_view_add_child;
}

/**
 * sp_visualizer_view_get_zoom_manager:
 *
 * Returns: (transfer none) (nullable): An #SpZoomManager or %NULL
 */
SpZoomManager *
sp_visualizer_view_get_zoom_manager (SpVisualizerView *self)
{
  SpVisualizerViewPrivate *priv = sp_visualizer_view_get_instance_private (self);

  g_return_val_if_fail (SP_IS_VISUALIZER_VIEW (self), NULL);

  return priv->zoom_manager;
}

void
sp_visualizer_view_set_zoom_manager (SpVisualizerView *self,
                                     SpZoomManager    *zoom_manager)
{
  SpVisualizerViewPrivate *priv = sp_visualizer_view_get_instance_private (self);

  g_return_if_fail (SP_IS_VISUALIZER_VIEW (self));
  g_return_if_fail (!zoom_manager || SP_IS_ZOOM_MANAGER (zoom_manager));

  if (g_set_object (&priv->zoom_manager, zoom_manager))
    {
      sp_visualizer_list_set_zoom_manager (priv->list, zoom_manager);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ZOOM_MANAGER]);
      gtk_widget_queue_resize (GTK_WIDGET (self));
    }
}
