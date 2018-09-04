project('vimway', 'c')


deps = [ dependency('pixman-1'),
  dependency('wayland-server'),
  dependency('wlroots') ]

inc_dir = include_directories('include/')

executable('vimway', ['src/main.c', 'src/vimway.c', 'include/vimway.h'], dependencies: deps, c_args: '-DWLR_USE_UNSTABLE', include_directories: inc_dir)
