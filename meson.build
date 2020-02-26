project('window', 'c')

sized_types_proj = subproject('sized_types')
sized_types = sized_types_proj.get_variable('sized_types_dep')

cc = meson.get_compiler('c')
dep_dir = meson.source_root() + '/deps'
libXau   = cc.find_library('Xau', dirs: [ dep_dir ])
libXdmcp = cc.find_library('Xdmcp', dirs: [ dep_dir ])
libxcb   = cc.find_library('xcb', dirs: [ dep_dir ])

c_args = [
  '-std=c90',
  '-pedantic-errors',
  '-Wall',
  '-Wpedantic'
]

libwindow = library(
  'window',
  'src/window_linux.c',
  c_args: c_args,
  include_directories: './include',
  dependencies: [
    libXau,
    libXdmcp,
    libxcb,
    sized_types
  ]
)

libwindow_dep = declare_dependency(
  include_directories: './include',
  link_with: [ libwindow ],
  dependencies: [ sized_types ]
)
