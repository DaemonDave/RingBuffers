.version:
	git describe --tags > build-aux/.version
	
dist-hook:
	git describe --tags  > $(distdir)/.tarball-version

.PHONY:  .version



commit:
	DATE='date'
	git commit -m  " $DATE "
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
		


