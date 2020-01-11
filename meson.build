project('libwindow', 'c')

sized_types_proj = subproject('sized_types')

sized_types = sized_types_proj.get_variable('sized_types_dep')

defines = []
target_os = get_option('target_os')
if target_os == 'linux'
  defines += '-DWINDOW_TARGET_OS_LINUX'
elif target_os == 'wasm'
  defines += '-DWINDOW_TARGET_OS_WASM'
endif

cc = meson.get_compiler('c')
dep_dir = meson.source_root() + '/deps'
libXau     = cc.find_library('Xau'     , dirs: [ dep_dir ])
libXdmcp   = cc.find_library('Xdmcp'   , dirs: [ dep_dir ])
libxcb     = cc.find_library('xcb'     , dirs: [ dep_dir ])
libxcb_shm = cc.find_library('xcb-shm' , dirs: [ dep_dir ])

c_args = [
  '-std=c90',
  '-pedantic-errors',
  '-Wall',
  '-Wconversion'
]
c_args += defines
libwindow = library(
  'window',
  'src/window_linux.c',
  c_args: c_args,
  dependencies: [
    libXau,
    libXdmcp,
    libxcb,
    libxcb_shm,
    sized_types
  ]
)

libwindow_include = include_directories('src')
libwindow_dep = declare_dependency(
  link_with: libwindow,
  include_directories: libwindow_include,
  dependencies: [
    sized_types
  ]
)
