srcdir = Dir('src')

env = Environment(CCFLAGS='-g -Wall -Wextra -Werror -pedantic', CFLAGS='-std=c99', CXXFLAGS='-std=c++98', CPPPATH=[srcdir], LIBPATH=[srcdir])
Export('env')

libm3conf = SConscript('src/SConscript')
test = SConscript('test/SConscript')
