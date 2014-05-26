SUBDIRS = shokparser

bin_SCRIPTS = shok_parser
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = shok_parser.in

do_substitution = sed -e 's,[@]pythondir[@],$(pythondir),g' \
	-e 's,[@]PACKAGE[@],$(PACKAGE),g' \
	-e 's,[@]VERSION[@],$(VERSION),g' 

shok_parser: shok_parser.in Makefile
	$(do_substitution) < $(srcdir)/shok_parser.in > shok_parser
	chmod +x shok_parser
