threads_dep = dependency('threads')

cpp = meson.get_compiler('cpp')

if cpp.get_id() == 'msvc'
  cpp_args = [
    '/wd4100',
    '/wd4201',
    '/wd4456',
    '/wd4457',
    '/wd4702',
    '/wd4815',
    '/utf-8',
  ]
  cpp_args = cpp.get_supported_arguments(cpp_args)
  add_project_arguments(cpp_args, language: 'cpp')
endif

if target_machine.system() == 'windows'
  cpp_args = [
    '-DUNICODE',
    '-D_UNICODE',
    '-DSTRICT',
    '-DNOMINMAX',
    '-D_CRT_SECURE_NO_WARNINGS',
    '-D_SCL_SECURE_NO_WARNINGS',
  ]
  add_project_arguments(cpp_args, language: 'cpp')
endif

sources = files(
  're2/re2/bitstate.cc',
  're2/re2/compile.cc',
  're2/re2/dfa.cc',
  're2/re2/filtered_re2.cc',
  're2/re2/mimics_pcre.cc',
  're2/re2/nfa.cc',
  're2/re2/onepass.cc',
  're2/re2/parse.cc',
  're2/re2/perl_groups.cc',
  're2/re2/prefilter.cc',
  're2/re2/prefilter_tree.cc',
  're2/re2/prog.cc',
  're2/re2/re2.cc',
  're2/re2/regexp.cc',
  're2/re2/set.cc',
  're2/re2/simplify.cc',
  're2/re2/stringpiece.cc',
  're2/re2/tostring.cc',
  're2/re2/unicode_casefold.cc',
  're2/re2/unicode_groups.cc',
  're2/util/rune.cc',
  're2/util/strutil.cc',
  'xsqlitestrings.cc',
)

sym_path = join_paths(meson.current_source_dir(), 'libxsqlitestrings.version')
ldflag = '-Wl,--version-script=' + sym_path

library(
  'xsqlitestrings',
  sources,
  dependencies: threads_dep,
  link_args: [ldflag],
  link_depends: [sym_path],
  install: true,
  install_dir: exts_dir,
  include_directories: include_directories('re2/')
)
