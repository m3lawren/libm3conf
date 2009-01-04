AC_INIT([libm3conf],[0.0.0],[m3lawren@uwaterloo.ca])
AM_INIT_AUTOMAKE([])

# Tools 
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
LT_INIT

# Data type checks
AC_C_CONST
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Function checks
AC_CHECK_FUNCS([strchr])
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_VPRINTF

# Arguments
AC_ARG_ENABLE(
	[full-debug], 
	AS_HELP_STRING([--enable-full-debug],[Enables full debug mode, which clobbers CFLAGS and CXXFLAGS, and will likely break on non-GCC compilers.]), 
	[AC_SUBST(
		[CFLAGS], 
		["-DM3CONF_DEBUG -Wall -Wextra -Werror -ansi -pedantic -g"])
	 AC_SUBST(
	 	[CXXFLAGS],
		["-DM3CONF_DEBUG -Wall -Wextra -Werror -ansi -pedantic -g"])],
	[])
AC_ARG_ENABLE(
	[full-check], 
	AS_HELP_STRING([--enable-full-check],[Enables full check mode. This is similar to --enable-full-debug, however it turns off debug output and enables optimization to try and catch extra errors.]),
	[AC_SUBST(
		[CFLAGS], 
		["-Wall -Wextra -Werror -ansi -pedantic -g -O2"])
	 AC_SUBST(
	 	[CXXFLAGS],
		["-Wall -Wextra -Werror -ansi -pedantic -g -O2"])],
	[])
AC_ARG_ENABLE(
	[coverage],
	AS_HELP_STRING([--enable-coverage],[Enables coverage checking. This clobbers CFLAGS and CXXFLAGS.]),
	[AC_SUBST(
		[CFLAGS],
		["-ftest-coverage -fprofile-arcs -g"])
	 AC_SUBST(
	 	[CXXFLAGS],
		["-ftest-coverage -fprofile-arcs -g"])],
	[])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	m3conf/Makefile
	test/Makefile
])

AC_OUTPUT
