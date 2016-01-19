default_context.basedir_object_file = 'tmp'
clang = find_external('clang', compile_cpp_opts = '-g -Weverything -Wno-c++98-compat-pedantic -Wno-padded -fno-omit-frame-pointer')
if clang:
	compiler['C++'] = clang
compiler['C++'].std = 'latest'

wx_widgets = find_external('wx')
sources = ['Hash.cpp', 'Crypt.cpp', 'Tiger.cpp']
default_targets = executable('minicrypt.bin', sources + ['MiniCrypt.cpp', 'I18N.cpp', wx_widgets])

if clang:
	for sanitizer in ['thread', 'address', 'undefined']:
		executable('test_%s.bin' % sanitizer, sources + ['test.cpp'],
			compiler_opts = '-fsanitize=' + sanitizer,
			linker_opts = '-fsanitize=' + sanitizer)
else:
	executable('test.bin', sources + ['test.cpp'])
