default_context.basedir_object_file = 'tmp'
use_toolchain('llvm', cpp_std = 'c++14',
	cpp_opts = '-g -Weverything -Wno-c++98-compat-pedantic -Wno-padded -fno-omit-frame-pointer')

wx_widgets = find_external('wx')
default_targets = executable('minicrypt.bin', ['MiniCrypt.cpp', 'Crypt.cpp', 'I18N.cpp', wx_widgets])

if tools['cpp'].name == 'clang':
	for sanitizer in ['thread', 'address', 'undefined']:
		executable('test_%s.bin' % sanitizer, ['Crypt.cpp', 'test.cpp'],
			compiler_opts = '-fsanitize=' + sanitizer,
			linker_opts = '-fsanitize=' + sanitizer)
main = executable('test.bin', ['Crypt.cpp', 'test.cpp'])

install([main], '~/bin')
