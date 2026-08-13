project('saver', 'cpp',
  version : '0.1.0',
  default_options : [
    'cpp_std=c++20',
    'warning_level=3',
    'buildtype=debugoptimized'
  ]
)

curl_dep    = dependency('libcurl', fallback : ['curl', 'curl_dep'])
crypto_dep  = dependency('libcrypto', fallback : ['openssl', 'libcrypto_dep'])
ssl_dep     = dependency('libssl', fallback : ['openssl', 'libssl_dep'])

sources = run_command('find', 'src', '-name', '*.cpp', check: true).stdout().strip().split('\n')
inc = include_directories('include', 'external')

executable('saver',
  sources,
  include_directories : inc,
  dependencies : [curl_dep, crypto_dep, ssl_dep]
)