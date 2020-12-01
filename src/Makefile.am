## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = gtkpopstation

#gtkpopstation$(EXEEXT): data/psx.db data/BASE.PBP

gtkpopstation_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	convert.c convert.h

EXTRA_gtkpopstation_DEPENDENCIES = data/psx.db data/BASE.PBP

gtkpopstation_LDFLAGS = -Wl,-b,binary,data/psx.db,data/BASE.PBP,-b,default

gtkpopstation_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) -lz

