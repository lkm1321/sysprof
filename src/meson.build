if get_option('enable_gtk')

sysprof_sources = [
  'sysprof.c',
  'sp-application.c',
  'sp-application.h',
  'sp-credits.h',
  'sp-window.c',
  'sp-window.h',
  'sp-window-settings.c',
  'sp-window-settings.h',
]

sysprof_resources = gnome.compile_resources(
  'sp-resources', 'resources/sysprof.gresource.xml',
  source_dir: 'resources',
  c_name: 'sysprof',
)

sysprof = executable('sysprof',
  sysprof_resources + sysprof_sources,
  dependencies: [
    libsysprof_ui_dep,
    cc.find_library('m', required: false),
  ],
  c_args: exe_c_args,
  link_args: exe_link_args,
  install: true,
)

endif
