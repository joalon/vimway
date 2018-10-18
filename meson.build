project('vimway', 'c')

cc = meson.get_compiler('c')
maths_dep = cc.find_library('m', required : true)
util_dep = cc.find_library('util', required : true)

deps = [
  dependency('pixman-1'),
  dependency('wayland-server'),
  dependency('wlroots'),
  dependency('xkbcommon'),
  dependency('threads'),
  dependency('libuv'),
  dependency('msgpack'),
  dependency('unibilium'),
  dependency('termkey'),
  dependency('lua'),
  dependency('luajit'),
  dependency('vterm'),
  maths_dep,
  util_dep
]

include_dirs= [
  include_directories('include/')
]

executable('vimway', ['src/main.c', 'src/vimway.c', 'include/vimway.h'], dependencies: deps, objects: 'include/libnvim.a', c_args: '-DWLR_USE_UNSTABLE', include_directories: include_dirs)
