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
json_dep    = dependency('nlohmann_json')

sources = run_command('find', 'src', '-name', '*.cpp', check: true).stdout().strip().split('\n')
inc = include_directories('include', 'external')

add_project_arguments(
  '-DAPP_RESOURCE_DIR="' + pkg_datadir / 'resources"',
  language : 'cpp'
)

executable('saver',
  sources,
  install: true,
  include_directories : inc,
  dependencies : [curl_dep, crypto_dep, ssl_dep, json_dep]
)