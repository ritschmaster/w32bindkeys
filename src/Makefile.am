################################################################################
# This file is part of w32bindkeys.
#
# Copyright 2020 Richard Paul Baeck <richard.baeck@mailbox.org>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
################################################################################


AM_CFLAGS = -D PKGDATADIR=\"$(pkgdatadir)\"

if DEBUG
AM_CFLAGS += -g -O0
AM_CFLAGS += -D DEBUG_ENABLED=1
AM_LDFLAGS = -g -O0
else
AM_CFLAGS += -O2
AM_LDFLAGS = -O2
endif

bin_PROGRAMS = w32bindkeys

if INSTALLLIBRARY
lib_LTLIBRARIES = libw32bindkeys.la
else
noinst_LTLIBRARIES = libw32bindkeys.la
endif


libw32bindkeys_la_SOURCES = logger.c logger.h
libw32bindkeys_la_SOURCES += util.c util.h
libw32bindkeys_la_SOURCES += datafinder.c datafinder.h
libw32bindkeys_la_SOURCES += be.c be.h
libw32bindkeys_la_SOURCES += b.c b.h
libw32bindkeys_la_SOURCES += kc.c kc.h
libw32bindkeys_la_SOURCES += kc_sys.c kc_sys.h
libw32bindkeys_la_SOURCES += kbman.c kbman.h
libw32bindkeys_la_SOURCES += kbdaemon.c kbdaemon.h
libw32bindkeys_la_SOURCES += parser.c parser.h

libw32bindkeys_la_CFLAGS = $(AM_CFLAGS)
libw32bindkeys_la_CFLAGS += @collectionc_CFLAGS@
libw32bindkeys_la_LDFLAGS = $(AM_LDFLAGS)
libw32bindkeys_la_LDFLAGS += -mwindows
libw32bindkeys_la_LDFLAGS += -static
libw32bindkeys_la_LDFLAGS += -lwtsapi32
libw32bindkeys_la_LIBADD = @collectionc_LIBS@

w32bindkeys_SOURCES = main.c
w32bindkeys_CFLAGS = $(AM_CFLAGS)
w32bindkeys_CFLAGS += @collectionc_CFLAGS@
w32bindkeys_LDFLAGS = $(AM_LDFLAGS)
w32bindkeys_LDFLAGS += -mwindows
w32bindkeys_LDFLAGS += -static
w32bindkeys_LDFLAGS += -lwtsapi32
w32bindkeys_LDADD = libw32bindkeys.la
w32bindkeys_LDADD += @collectionc_LIBS@

if INSTALLLIBRARY
install-data-local:
	mkdir -p $(libdir)/pkgconfig
	$(INSTALL_DATA) libw32bindkeys.pc $(libdir)/pkgconfig/
	sed -i s,AUTOMAKE_PREFIX,$(prefix),g $(libdir)/pkgconfig/libw32bindkeys.pc
	sed -i s,AUTOMAKE_VERSION,$(PACKAGE_VERSION),g $(libdir)/pkgconfig/libw32bindkeys.pc

install-exec-local:
	echo Hello

uninstall-local:
	rm -f $(libdir)/pkgconfig/libw32bindkeys.pc
endif

SUBDIRS = headers_installation

