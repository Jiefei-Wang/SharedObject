AC_INIT([SharedObject], 1.0.0) dnl package name, autoconfig file version

dnl check librt for shm_open support
dnl R usually is linked to librt but not always(eg. Ubuntu 20.04)
AC_CHECK_LIB(rt, shm_open)
AC_SUBST(LIBS)
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT