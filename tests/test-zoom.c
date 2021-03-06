#include "util/sp-zoom-manager.h"

static void
test_zoom_manager (void)
{
  SpZoomManager *zoom;

  zoom = sp_zoom_manager_new ();
  g_assert_cmpfloat (1.0, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_in (zoom);
  g_assert_cmpfloat (1.1, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_in (zoom);
  g_assert_cmpfloat (1.2, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_in (zoom);
  g_assert_cmpfloat (1.33, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_out (zoom);
  g_assert_cmpfloat (1.2, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_out (zoom);
  g_assert_cmpfloat (1.1, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_out (zoom);
  g_assert_cmpfloat (1.0, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_out (zoom);
  g_assert_cmpfloat (.9, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_out (zoom);
  g_assert_cmpfloat (.8, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_out (zoom);
  g_assert_cmpfloat (.67, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_out (zoom);
  g_assert_cmpfloat (.5, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_out (zoom);
  g_assert_cmpfloat (.3, ==, sp_zoom_manager_get_zoom (zoom));
  sp_zoom_manager_zoom_out (zoom);
  g_assert_cmpfloat (.3 / 2, ==, sp_zoom_manager_get_zoom (zoom));

  g_object_unref (zoom);
}


gint
main (gint argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/ZoomManager/basic", test_zoom_manager);
  return g_test_run ();
}
