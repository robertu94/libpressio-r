AC_INIT([libpressio], 0.0.1)
AC_CONFIG_HEADERS([config.h])

#Find the compiler and flags used by R
: ${R_HOME=`R RHOME`}
if test -z "${R_HOME}"; then
  echo "could not determine R_HOME"
  exit 1
fi
CC=`"${R_HOME}/bin/R" CMD config CC`
CXX=`"${R_HOME}/bin/R" CMD config CXX`
CFLAGS=`"${R_HOME}/bin/R" CMD config CFLAGS`
CPPFLAGS=`"${R_HOME}/bin/R" CMD config CPPFLAGS`
CXXFLAGS=`"${R_HOME}/bin/R" CMD config CXXFLAGS`
AC_LANG(C++)


PKG_CHECK_MODULES([LibPressio], [libpressio_cxx])
AC_SUBST(LibPressio_CFLAGS)
AC_SUBST(LibPressio_LIBS)
PKG_CHECK_MODULES([LibPressioMeta], [libpressio_meta_cxx], HAVE_LIBPRESSIO_META="yes", HAVE_LIBPRESSIO_META="no")
AC_SUBST(LibPressioMeta_CFLAGS)
AC_SUBST(LibPressioMeta_LIBS)
AM_CONDITIONAL(HAVE_LIBPRESSIO_META, test "x$HAVE_LIBPRESSIO_META" == xyes)
if test "x$HAVE_LIBPRESSIO_META" == "xyes"; then
  AC_DEFINE(HAVE_LIBPRESSIO_META, [], [build support for libpressio 3rd party modules])
fi

AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT
