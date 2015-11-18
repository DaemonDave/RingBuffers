#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Library Version Master Version DO NOT TOUCH
MAJORVERSION=0
MINORVERSION=2
REVISION=0
VER='${MAJORVERSION}:${MINORVERSION}:${REVISION}'
#
LIB_VER = ${VER}


REL_VER=${REVISION}
LIB_VER=${MAJORVERSION}.${MINORVERSION}-${REVISION}
#          |               |                 |
#   +------+               |                 +----+
#   |                      |                      |
#  current    :         revision     :  	age
#   |                      |                      |
#   |                      |                      +- increment if interfaces have been added
#   |                      |                         set to zero if interfaces have been removed
#   |                      |                         or changed
#   |                      +- increment if source code has changed
#   |                         set to zero if current is incremented
#   +- increment if interfaces have been added, removed or changed



#m4_esyscmd([echo $LIB_VER > .version])

AC_PREREQ([2.69])
AC_INIT([libringbuffers], m4_esyscmd([build-aux/git-version-gen.sh .tarball-version]), [evergent@shaw.ca])

AC_CONFIG_SRCDIR([src/ringbuffer.c])
AC_CONFIG_HEADERS([config.h])


# determine system type by conventional m4 macro
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])


AC_SUBST(VER)
AC_SUBST(MAJORVERSION)
AC_SUBST(MINORVERSION)
AC_SUBST(REVISION)
AC_SUBST(LIB_VER)
AC_SUBST(REL_VER)

# Get the date timestamp for library / documentation version control
DATE=$(date)
AC_SUBST(DATE)


# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR

LT_INIT



ATLAS_CFLAGS= 
 
AC_CHECK_HEADERS([atlas/atlas_buildinfo.h])
AC_CHECK_HEADERS([atlas/cblas.h])
AC_CHECK_HEADERS([atlas/clapack.h])
AC_CHECK_HEADERS([atlas/atlas_csysinfo.h])

ax_atlas_save_LIBS="$LIBS"
# 
ATLAS_CFLAGS=$(pkg-config --cflags atlas)
ATLAS_CFLAGS+=$(pkg-config --cflags blas-atlas)
ATLAS_CFLAGS+=$(pkg-config --cflags lapack-atlas)
ATLAS_LDFLAGS=$(pkg-config --libs atlas)
ATLAS_LDFLAGS+=$(pkg-config --libs blas-atlas)
ATLAS_LDFLAGS+=$(pkg-config --libs lapack-atlas)

AC_SUBST(ATLAS_CFLAGS)
AC_SUBST(ATLAS_LDFLAGS)



# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([Makefile  src/Makefile])
AC_OUTPUT


dnl echo ${PACKAGE_STRING} "library version is... " ${PACKAGE_VERSION}
dnl echo ${PACKAGE_STRING} "library release is... " ${MAJORVERSION}

AC_MSG_NOTICE([ ${PACKAGE_STRING} library version is...  = ${CONFIGURED_PROGRAMS}])
AC_MSG_NOTICE([ ${PACKAGE_STRING} library version is...  = ${REL_VER}])
AC_MSG_NOTICE([ CONFIGURED_PROGRAMS = ${CONFIGURED_PROGRAMS}])
AC_MSG_NOTICE([ EXTRA_CHECK_PROGRAMS = ${EXTRA_CHECK_PROGRAMS}])
#AC_MSG_NOTICE([ CONFIGURED_TESTS = ${CONFIGURED_TESTS}])
#AC_MSG_NOTICE([ CONFIGURED_STATIC_LIBRARIES = ${CONFIGURED_STATIC_LIBRARIES}])
#AC_MSG_NOTICE([ CONFIGURED_SUBDIRS = ${CONFIGURED_SUBDIRS}])
#AC_MSG_NOTICE([ RCSLIB_EXTRA_OBJS = ${RCSLIB_EXTRA_OBJS} ])
#AC_MSG_NOTICE([ RCSRTAILIB_EXTRA_OBJS = ${RCSRTAILIB_EXTRA_OBJS} ])
AC_MSG_NOTICE([ CPPFLAGS = ${CPPFLAGS} ])
AC_MSG_NOTICE([ CFLAGS = ${CFLAGS} ])
AC_MSG_NOTICE([ CXXFLAGS = ${CXXFLAGS} ])
AC_MSG_NOTICE([ LDFLAGS = ${LDFLAGS} ])
AC_MSG_NOTICE([ LIBS = ${LIBS} ])
AC_MSG_NOTICE([ CC = ${CC} ])
AC_MSG_NOTICE([ CXX = ${CXX} ])
AC_MSG_NOTICE([ ATLAS_CFLAGS = ${ATLAS_CFLAGS} ])
AC_MSG_NOTICE([ ATLAS_LDFLAGS = ${ATLAS_LDFLAGS} ])

