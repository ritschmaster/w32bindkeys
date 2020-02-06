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

if DEBUG
AM_CFLAGS = -g3 -O0
AM_LDFLAGS = -g3 -O0
endif

bin_PROGRAMS = w32bindkeys

noinst_LTLIBRARIES = libwbkinterpreter.la
if INSTALLBASE
lib_LTLIBRARIES = libwbkbase.la
else
noinst_LTLIBRARIES += libwbkbase.la
endif

nobase_include_HEADERS = wbkbase/logger.h
nobase_include_HEADERS += wbkbase/util.h
nobase_include_HEADERS += wbkbase/kb.h
nobase_include_HEADERS += wbkbase/kbman.h
nobase_include_HEADERS += wbkbase/ikbman.h
nobase_include_HEADERS += wbkbase/parser.h

#libwbkbase_la_SOURCES = logger.h
libwbkbase_la_SOURCES = logger.c
#libwbkbase_la_SOURCES += util.h
libwbkbase_la_SOURCES += util.c
#libwbkbase_la_SOURCES += kb.h
libwbkbase_la_SOURCES += kb.c
#libwbkbase_la_SOURCES += kbman.h
libwbkbase_la_SOURCES += kbman.c
#libwbkbase_la_SOURCES += ikbman.h
libwbkbase_la_SOURCES += ikbman.c
#libwbkbase_la_SOURCES += parser.h
libwbkbase_la_SOURCES += parser.c

libwbkbase_la_CFLAGS = @collectionc_CFLAGS@
libwbkbase_la_LDFLAGS = --static -mwindows
libwbkbase_la_LIBADD = @collectionc_LIBS@

libwbkinterpreter_la_SOURCES = iparser.h
libwbkinterpreter_la_SOURCES += iparser.c

libwbkinterpreter_la_CFLAGS = @collectionc_CFLAGS@
libwbkinterpreter_la_LDFLAGS = --static -mwindows
libwbkinterpreter_la_LIBADD = libwbkbase.la @collectionc_LIBS@

w32bindkeys_SOURCES = main.c
w32bindkeys_LDFLAGS = --static
w32bindkeys_LDADD = libwbkinterpreter.la


