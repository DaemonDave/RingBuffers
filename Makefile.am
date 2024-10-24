.version:
	git describe --tags > build-aux/.version
	
dist-hook:
	git describe --tags  > $(distdir)/.tarball-version

.PHONY:  .version git-clean

# DRE 2024 - added to make it easier to git commit  git pull by clearing untracked files:
git-clean:
	rm -fr Doxyfile ar-lib autom4te.cache/output.* autom4te.cache/traces.* config.h config.h.in~ config.log config.status configure~ depcomp libtool src/.deps/ src/Makefile src/orig/ stamp-h1
#NOTE: Kept Makefile and added to the repository so you can use these Makefile recipes: DRE 2024
	

commit:
	DATE='date'
	git commit -m  " $DATE " -a
	git push 
	
.PHONY: commit
	

tag:
	git tag  @LIB_VER@
 
.PHONY: tag

DISTCLEANFILES = config.h.in~ \
				ar-lib \
				autom4te.cache/output.* \
				autom4te.cache/requests \
				autom4te.cache/traces.* \
				depcomp
									
									


SUBDIRS = src

ACLOCAL_AMFLAGS = 	-I m4

EXTRA_DIST= \
		.git/*  \
		.gitignore \
		COPYING \
		NEWS \
		README \
		build-aux/git-version-gen.sh \
		VERSION \
		build-aux/.gitignore \
		m4/m4_ax_atlas.m4


# simple doxygen target for pre-existing Doxyfile.in template filled in
dox: Doxyfile.in
	doxygen  Doxyfile
	


