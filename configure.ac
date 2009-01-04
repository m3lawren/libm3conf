AC_INIT([libm3conf], [0.0.0], [m3lawren@uwaterloo.ca])
AM_INIT_AUTOMAKE([])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LIBTOOL
LT_INIT

AC_CHECK_FUNCS([strchr])
AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_TYPE_SIZE_T

AC_ARG_ENABLE(
	[full-debug], 
	AC_HELP_STRING(
		[--enable-full-debug], 
		[Enables full debug mode, which clobbers CFLAGS and CXXFLAGS, and will likely break on non-GCC compilers.]), 
	[AC_SUBST(
		[CFLAGS], 
		["-DM3CONF_DEBUG -Wall -Wextra -Werror -ansi -pedantic -g"])], 
	[])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	m3conf/Makefile
	test/Makefile
])

AC_OUTPUT
