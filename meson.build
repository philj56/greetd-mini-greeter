project(
  'greetd-mini-greeter',
  'c',
  license: 'MIT',
  default_options: [
    'c_std=c11',
    'optimization=3',
    'buildtype=release',
    'warning_level=3',
    'b_lto=true',
    'b_lto_threads=-1',
    'b_pie=true',
    'prefix=/usr'
  ],
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
  ],
  language: 'c'
)

sources = files(
  'src/main.c',
  'src/ipc.c',
  'src/greetd.c'
)

json = dependency('json-c')
gtk = dependency('gtk4')
thread = dependency('threads')

executable(
  'greetd-mini-greeter',
  sources,
  dependencies: [json, gtk, thread],
  install: true
)
