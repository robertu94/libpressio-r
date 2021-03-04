AC_INIT([libpressio], 0.0.1)

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
AC_PROG_CPP


PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([LibPressio], [libpressio])
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT
