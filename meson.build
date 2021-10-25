project(
  'greetd-mini-greeter',
  'c',
  license: 'MIT',
  default_options: [
    'c_std=c2x',
    'optimization=3',
    'buildtype=release',
    'warning_level=3',
    'b_lto=true',
    'b_lto_threads=-1',
    'b_pie=true',
    'prefix=/usr'
  ],
)

debug = get_option('buildtype').startswith('debug')

data_location = join_paths(
  get_option('prefix'),
  get_option('datadir'),
  'greetd-mini-greeter'
)

ui_location = join_paths(
  debug ? '' : data_location,
  'greeter.ui'
)

gtk3_ui_location = join_paths(
  debug ? '' : data_location,
  'greeter-gtk3.ui'
)

css_location = join_paths(
  debug ? '' : data_location,
  'greeter.css'
)

add_project_arguments(
  [
    '-pedantic',
    #'-Wconversion',
    '-Wshadow',
    '-Wno-unused-parameter',
    '-D_POSIX_C_SOURCE=200809L',
    '-DGDK_DISABLE_DEPRECATED',
    '-DGTK_DISABLE_DEPRECATED',
    '-DGTK_UI_PATH="@0@"'.format(ui_location),
    '-DGTK3_UI_PATH="@0@"'.format(gtk3_ui_location),
    '-DGTK_CSS_PATH="@0@"'.format(css_location),
  ],
  language: 'c'
)

common_sources = files(
  'src/ipc.c',
  'src/greetd.c'
)

gtk3_sources = files(
  'src/gtk3/main.c',
)

gtk4_sources = files(
  'src/gtk4/main.c',
)

json = dependency('json-c')
gtk3 = dependency('gtk+-3.0')
gtk4 = dependency('gtk4')

executable(
  'greetd-mini-greeter',
  [common_sources, gtk4_sources],
  dependencies: [json, gtk4],
  install: true
)

executable(
  'greetd-mini-greeter-gtk3',
  [common_sources, gtk3_sources],
  dependencies: [json, gtk3],
  install: true
)

install_data('greeter.ui')
install_data('greeter-gtk3.ui')
install_data('greeter.css')

scdoc = find_program('scdoc', required: get_option('man-pages'))
if scdoc.found()
  sed = find_program('sed')
  sh = find_program('sh')
  mandir = get_option('mandir')
  manpage = 'doc/greetd-mini-greeter.1.scd'
  output = 'greetd-mini-greeter.1'
  custom_target(
    output,
    input: manpage,
    output: output,
    command: [
      sh, '-c', '@0@ < @INPUT@ | sed "s|CSS_PATH|@1@|g;" > @2@'.format(scdoc.path(), css_location, output)
    ],
    install: true,
    install_dir: '@0@/man1'.format(mandir)
  )
endif
