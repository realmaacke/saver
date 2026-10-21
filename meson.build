project('saver', 'cpp',
  version : '0.1.0',
  default_options : [
    'cpp_std=c++20',
    'warning_level=3',
    'buildtype=debugoptimized'
  ]
)

pkg_datadir = get_option('prefix') / get_option('datadir') / meson.project_name()

# Installs the folder to: <prefix>/share/saver/resources
install_subdir('resources', install_dir : pkg_datadir)

curl_dep    = dependency('libcurl', fallback : ['curl', 'curl_dep'])
crypto_dep  = dependency('libcrypto', fallback : ['openssl', 'libcrypto_dep'])
ssl_dep     = dependency('libssl', fallback : ['openssl', 'libssl_dep'])
json_dep    = dependency('nlohmann_json', fallback : ['nlohmann_json', 'nlohmann_json_dep'])
gtest_dep = dependency('gtest_main', fallback : ['gtest', 'gtest_main_dep'])
gmock_dep = dependency('gmock', fallback : ['gtest', 'gmock_dep'])

add_project_arguments(
  '-DAPP_RESOURCE_DIR="' + pkg_datadir / 'resources"',
  language : 'cpp'
)

sources = run_command('find', 'src', '-name', '*.cpp', check: true).stdout().strip().split('\n')
inc = include_directories('include', 'external')


# Everything except main goes into a library so tests can link it
lib_sources = []
foreach s : sources
  if s != 'src/main.cpp'
    lib_sources += s
  endif
endforeach

app_deps = [curl_dep, crypto_dep, ssl_dep, json_dep]

mock_inc = include_directories('tests/mocks')

saver_lib = static_library('saver_core',
  lib_sources,
  include_directories: inc,
  dependencies: app_deps
)

saver_lib_test = static_library('saver_core_test',
  lib_sources,
  include_directories : [mock_inc, inc],
  dependencies : app_deps
)

executable('saver',
  'src/main.cpp',
  install: false,
  include_directories : inc,
  link_with : saver_lib,
  dependencies : app_deps
)


tests = run_command('find', 'tests', '-name', '*.cpp', check: true).stdout().strip().split('\n')
test_exe = executable('tests',
  tests,
  include_directories : [mock_inc, inc],
  link_with : saver_lib_test,
  dependencies : [gtest_dep, gmock_dep] + app_deps
)
test('app_unit_tests', test_exe)