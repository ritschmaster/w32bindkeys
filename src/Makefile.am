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
AM_LDFLAGS = -g -O0
endif

bin_PROGRAMS = w32bindkeys

noinst_LTLIBRARIES = libwbkinterpreter.la
if INSTALLBASE
lib_LTLIBRARIES = libwbkbase.la
else
noinst_LTLIBRARIES += libwbkbase.la
endif

if INSTALLBASE
nobase_include_HEADERS = wbkbase/logger.h
nobase_include_HEADERS += wbkbase/util.h
nobase_include_HEADERS += wbkbase/kb.h
nobase_include_HEADERS += wbkbase/kbman.h
nobase_include_HEADERS += wbkbase/ikbman.h
nobase_include_HEADERS += wbkbase/parser.h
endif

libwbkbase_la_SOURCES = logger.c logger.h
libwbkbase_la_SOURCES += util.c util.h
libwbkbase_la_SOURCES += datafinder.c datafinder.h
libwbkbase_la_SOURCES += kb.c kb.h
libwbkbase_la_SOURCES += kbman.c kbman.h
libwbkbase_la_SOURCES += ikbman.c ikbman.h
libwbkbase_la_SOURCES += parser.c parser.h

libwbkbase_la_CFLAGS = @collectionc_CFLAGS@
libwbkbase_la_LDFLAGS = --static -mwindows
libwbkbase_la_LIBADD = @collectionc_LIBS@

libwbkinterpreter_la_SOURCES = iparser.c iparser.h

libwbkinterpreter_la_CFLAGS = @collectionc_CFLAGS@
libwbkinterpreter_la_LDFLAGS = --static -mwindows
libwbkinterpreter_la_LIBADD = libwbkbase.la @collectionc_LIBS@

w32bindkeys_SOURCES = main.c
w32bindkeys_LDFLAGS = --static
w32bindkeys_LDADD = libwbkinterpreter.la

if INSTALLBASE
install-data-local:
	mkdir -p $(libdir)/pkgconfig
	$(INSTALL_DATA) libwbkbase.pc $(libdir)/pkgconfig/
	sed -i s,AUTOMAKE_PREFIX,$(prefix),g $(libdir)/pkgconfig/libwbkbase.pc
	sed -i s,AUTOMAKE_VERSION,$(PACKAGE_VERSION),g $(libdir)/pkgconfig/libwbkbase.pc

uninstall-local:
	rm -f $(libdir)/pkgconfig/libwbkbase.pc
endif
