# ===========================================================================
#          http://www.gnu.org/software/autoconf-archive/ax_blas.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BLAS([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]])
#
# DESCRIPTION
#
#   This macro looks for a library that implements the BLAS linear-algebra
#   interface (see http://www.netlib.org/blas/). On success, it sets the
#   ATLAS_LIBS output variable to hold the requisite library linkages.
#
#   To link with BLAS, you should link with:
#
#     $ATLAS_LIBS $LIBS $FLIBS
#
#   in that order. FLIBS is the output variable of the
#   AC_F77_LIBRARY_LDFLAGS macro (called if necessary by AX_BLAS), and is
#   sometimes necessary in order to link with F77 libraries. Users will also
#   need to use AC_F77_DUMMY_MAIN (see the autoconf manual), for the same
#   reason.
#
#   Many libraries are searched for, from ATLAS to CXML to ESSL. The user
#   may also use --with-blas=<lib> in order to use some specific BLAS
#   library <lib>. In order to link successfully, however, be aware that you
#   will probably need to use the same Fortran compiler (which can be set
#   via the F77 env. var.) as was used to compile the BLAS library.
#
#   ACTION-IF-FOUND is a list of shell commands to run if a BLAS library is
#   found, and ACTION-IF-NOT-FOUND is a list of commands to run it if it is
#   not found. If ACTION-IF-FOUND is not specified, the default action will
#   define HAVE_BLAS.
#
# LICENSE
#
#   Copyright (c) 2008 Steven G. Johnson <stevenj@alum.mit.edu>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 14

AU_ALIAS([ACX_ATLAS], [AX_ATLAS])
AC_DEFUN([AX_ATLAS], [
AC_PREREQ(2.50)
AC_REQUIRE([AC_CANONICAL_HOST])
ax_atlas_ok=no

AC_ARG_WITH(atlas,
	[AS_HELP_STRING([--with-atlas=<lib>], [use ATLAS library <lib>])])
case $with_atlas in
	yes | "") ;;
	no) ax_atlas_ok=disable ;;
	-* | */* | *.a | *.so | *.so.* | *.o) ATLAS_LIBS="$with_atlas" ;;
	*) ATLAS_LIBS="-l$with_atlas" ;;
esac

ATLAS_CFLAGS= 
 
AC_CHECK_HEADERS([atlas/atlas_buildinfo.h])
AC_CHECK_HEADERS([atlas/atlas_csysinfo.h])

ax_atlas_save_LIBS="$LIBS"
# 
ATLAS_CFLAGS='pkg-config --cflags atlas'
ATLAS_LDFLAGS='pkg-config --libs atlas'


## 64-bit
#if test $host_cpu = x86_64; then
#		AC_CHECK_LIB(mkl_intel_lp64, $sgemm,
#			[ax_atlas_ok=yes;ATLAS_LIBS="-lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread"],,
#			[-lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread])
## 32-bit
#elif test $host_cpu = i686; then
#		AC_CHECK_LIB(mkl_intel, $sgemm,
#			[ax_atlas_ok=yes;ATLAS_LIBS="-lmkl_intel -lmkl_sequential -lmkl_core -lpthread"],,
#			[-lmkl_intel -lmkl_sequential -lmkl_core -lpthread])
#fi

# BLAS in ATLAS library? (http://math-atlas.sourceforge.net/)
if test $ax_atlas_ok = no; then
	AC_CHECK_LIB(atlas, ATL_xerbla,
		[AC_CHECK_LIB(f77blas, $sgemm,
		[AC_CHECK_LIB(cblas, cblas_dgemm,
			[ax_atlas_ok=yes
			 ATLAS_LIBS="-lcblas -lf77blas -latlas"],
			[], [-lf77blas -latlas])],
			[], [-latlas])])
fi



AC_SUBST(ATLAS_LIBS)

LIBS="$ax_blas_save_LIBS"

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$ax_atlas_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_ATLAS,1,[Define if you have a ATLAS library.]),[$1])
        :
else
        ax_atlas_ok=no
        $2
fi
])dnl AX_BLAS
