cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: false)

library(
  'xsqlitemath',
  ['xsqlitemath.c'],
  dependencies: m_dep,
  install: true,
  install_dir: exts_dir,
)
