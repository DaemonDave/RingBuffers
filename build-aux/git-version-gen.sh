#!/bin/sh
# Print a version string.
scriptversion=2011-02-19.19; # UTC

# Copyright (C) 2007-2011 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# This script is derived from GIT-VERSION-GEN from GIT: http://git.or.cz/.
# It may be run two ways:
# - from a git repository in which the "git describe" command below
#   produces useful output (thus requiring at least one signed tag)
# - from a non-git-repo directory containing a .tarball-version file, which
#   presumes this script is invoked like "./git-version-gen .tarball-version".

# In order to use intra-version strings in your project, you will need two
# separate generated version string files:
#
# .tarball-version - present only in a distribution tarball, and not in
#   a checked-out repository.  Created with contents that were learned at
#   the last time autoconf was run, and used by git-version-gen.  Must not
#   be present in either $(srcdir) or $(builddir) for git-version-gen to
#   give accurate answers during normal development with a checked out tree,
#   but must be present in a tarball when there is no version control system.
#   Therefore, it cannot be used in any dependencies.  GNUmakefile has
#   hooks to force a reconfigure at distribution time to get the value
#   correct, without penalizing normal development with extra reconfigures.
#
# .version - present in a checked-out repository and in a distribution
#   tarball.  Usable in dependencies, particularly for files that don't
#   want to depend on config.h but do want to track version changes.
#   Delete this file prior to any autoconf run where you want to rebuild
#   files to pick up a version string change; and leave it stale to
#   minimize rebuild time after unrelated changes to configure sources.
#
# It is probably wise to add these two files to .gitignore, so that you
# don't accidentally commit either generated file.
#
# Use the following line in your configure.ac, so that $(VERSION) will
# automatically be up-to-date each time configure is run (and note that
# since configure.ac no longer includes a version string, Makefile rules
# should not depend on configure.ac for version updates).
#
# AC_INIT([GNU project],
#         m4_esyscmd([build-aux/git-version-gen .tarball-version]),
#         [bug-project@example])
#
# Then use the following lines in your Makefile.am, so that .version
# will be present for dependencies, and so that .tarball-version will
# exist in distribution tarballs.
#
# BUILT_SOURCES = $(top_srcdir)/.version
# $(top_srcdir)/.version:
#	echo $(VERSION) > $@-t && mv $@-t $@
# dist-hook:
#	echo $(VERSION) > $(distdir)/.tarball-version

case $# in
    1|2) ;;
    *) echo 1>&2 "Usage: $0 \$srcdir/.tarball-version" \
         '[TAG-NORMALIZATION-SED-SCRIPT]'
       exit 1;;
esac

tarball_version_file=$1
tag_sed_script="${2:-s/x/x/}"
nl='
'

# Avoid meddling by environment variable of the same name.
v=
v_from_git=

# First see if there is a tarball-only version file.
# then try "git describe", then default.
if test -f $tarball_version_file
then
# DRE NOV 2015 - 
#    v=`cat $tarball_version_file` || v=
    v=`git describe --tags` || v=
    case $v in
        *$nl*) v= ;; # reject multi-line output
        [0-9]*) ;;
        *) v= ;;
    esac
    test -z "$v" \
        && echo "$0: WARNING: $tarball_version_file is missing or damaged" 1>&2
fi

if test -n "$v"
then
    : # use $v
# Otherwise, if there is at least one git commit involving the working
# directory, and "git describe" output looks sensible, use that to
# derive a version string.
elif test "`git log -1 --pretty=format:x . 2>&1`" = x \
    && v=`git describe --abbrev=4 --match='v*' --tags HEAD 2>/dev/null \
          || git describe --abbrev=4 --tags HEAD 2>/dev/null` \
    && v=`printf '%s\n' "$v" | sed "$tag_sed_script"` \
    && case $v in
         v[0-9]*) ;;
         *) (exit 1) ;;
       esac
then
    # Is this a new git that lists number of commits since the last
    # tag or the previous older version that did not?
    #   Newer: v6.10-77-g0f8faeb
    #   Older: v6.10-g0f8faeb
    case $v in
        *-*-*) : git describe is okay three part flavor ;;
        *-*)
            : git describe is older two part flavor
            # Recreate the number of commits and rewrite such that the
            # result is the same as if we were using the newer version
            # of git describe.
            vtag=`echo "$v" | sed 's/-.*//'`
            commit_list=`git rev-list "$vtag"..HEAD 2>/dev/null` \
                || { commit_list=failed;
                     echo "$0: WARNING: git rev-list failed" 1>&2; }
            numcommits=`echo "$commit_list" | wc -l`
            v=`echo "$v" | sed "s/\(.*\)-\(.*\)/\1-$numcommits-\2/"`;
            test "$commit_list" = failed && v=UNKNOWN
            ;;
    esac

    # Change the first '-' to a '.', so version-comparing tools work properly.
    # Remove the "g" in git describe's output string, to save a byte.
    v=`echo "$v" | sed 's/-/./;s/\(.*\)-g/\1-/'`;
    v_from_git=1
else
    v=UNKNOWN
fi

v=`echo "$v" |sed 's/^v//'`

# Test whether to append the "-dirty" suffix only if the version
# string we're using came from git.  I.e., skip the test if it's "UNKNOWN"
# or if it came from .tarball-version.
if test -n "$v_from_git"; then
  # Don't declare a version "dirty" merely because a time stamp has changed.
  git update-index --refresh > /dev/null 2>&1

  dirty=`exec 2>/dev/null;git diff-index --name-only HEAD` || dirty=
  case "$dirty" in
      '') ;;
      *) # Append the suffix only if there isn't one already.
          case $v in
            *-dirty) ;;
            *) v="$v-dirty" ;;
          esac ;;
  esac
fi

# Omit the trailing newline, so that m4_esyscmd can use the result directly.
echo "$v" | tr -d "$nl"

# Local variables:
# eval: (add-hook 'write-file-hooks 'time-stamp)
# time-stamp-start: "scriptversion="
# time-stamp-format: "%:y-%02m-%02d.%02H"
# time-stamp-time-zone: "UTC"
# time-stamp-end: "; # UTC"
# End:
