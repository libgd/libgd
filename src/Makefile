# $Id$
DIST		= gd
# Remeber to update version in the perl-file too!
VERMAJ		= 1
VERMIN		= 2
VERSION		= $(VERMAJ).$(VERMIN)

BASEDIR		= /usr/local
BINDIR		= $(BASEDIR)/bin
MANDIR		= $(BASEDIR)/man/man1
INSTALLDATA	= install -m 644
INSTALLBIN	= install -m 755

all:
	@echo Type \`make install\' to install $(DIST) in $(BASEDIR)

install:
	$(INSTALLBIN) gdprog $(BINDIR)
	$(INSTALLDATA) gdprog.1 gd.1 $(MANDIR)

clean:
	rm -f *~

veryclean: clean
	rm -f $(DIST)-$(VERSION).tar.gz $(DIST)-$(VERSION).lsm

chmod:
	chmod a+r *
	chmod a+x gdprog

# To let the author make a distribution. The rest of the Makefile
# should be used by the author only.
LSMFILE		= $(DIST)-$(VERSION).lsm
DISTDIR		= $(DIST)-$(VERSION)
DISTFILE	= $(DIST)-$(VERSION).tar.gz
DISTFILES	= README INSTALL $(LSMFILE) \
		  Makefile gdprog gd.1 gdprog.1 gd.ver

$(LSMFILE): FORCE
	VER=$(VERSION); \
	DATE=`date "+%d%b%y"|tr '[a-z]' '[A-Z]'`; \
	sed -e "s/VER/$$VER/g;s/DATE/$$DATE/g" $(DIST).lsm.in > $(LSMFILE)

FORCE:

# Warning: distclean removes the lsm-file, which can not be
# regenerated without the lsm.in-file, which is not part of the package.
distclean: veryclean
	rm -f $(LSMFILE)

dist: $(LSMFILE) chmod
	mkdir $(DISTDIR)
	chmod a+rx $(DISTDIR)
	ln $(DISTFILES) $(DISTDIR)
	tar -cvzf $(DISTFILE) $(DISTDIR)
	chmod a+r $(DISTFILE)
	rm -rf $(DISTDIR)
