import subprocess

def Settings( **kwargs ):
  flags = [
    '-x',
    'c++',
    '-Wall',
    '-Wextra',
    '-Wno-unused-parameter',
    '-std=c++14',

    '-I',
    '.',

    '-I', 'third_party/googletest/googletest/include',
    '-I', 'third_party/abseil-cpp',
    '-I', 'third_party/libbcf',
    '-pthread',
  ]

  flags += subprocess.check_output(['ncursesw5-config', '--cflags']).split('\n')
  return {
    'flags': flags,
  }
