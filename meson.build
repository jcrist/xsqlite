project(
  'xsqlite', 'c', 'cpp',
  default_options : [
    'c_std=c99',
    'cpp_std=c++11',
  ]
)

py = import('python')
py3 = py.find_installation(pure: false)

py_sources = [
  'src/xsqlite/__init__.py',
  'src/xsqlite/_core.py'
]

py3.install_sources(
  py_sources,
  subdir: 'xsqlite'
)

exts_dir = py3.get_install_dir() / 'xsqlite'/ 'exts'

subdir('exts')
