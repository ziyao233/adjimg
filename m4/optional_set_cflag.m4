define([OPTIONAL_SET_CFLAGS_INIT],
[
	old_CFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS -Werror"

	AC_MSG_CHECKING([Whether compiler supports -Werror])

	AC_COMPILE_IFELSE([AC_LANG_SOURCE[int foo(int a) { return a; }]], [
		AC_MSG_RESULT([yes])
		optional_set_cflags_extra_flags="-Werror"
	], [
		AC_MSG_RESULT([no])
		optional_set_cflags_extra_flags=""
	])

	CFLAGS="$old_CFLAGS"
])

define([OPTIONAL_SET_CFLAGS],
[
	old_CFLAGS="$CFLAGS"
	new_CFLAGS="$CFLAGS $1"
	CFLAGS="$new_CFLAGS $optional_set_cflags_extra_flags"

	AC_MSG_CHECKING([Whether compiler supports $1])

	AC_COMPILE_IFELSE([AC_LANG_SOURCE[int foo(int a) { return a; }]], [
		AC_MSG_RESULT([yes])
		CFLAGS="$new_CFLAGS"
	], [
		AC_MSG_RESULT([no])
		CFLAGS="$old_CFLAGS"
	])
])
