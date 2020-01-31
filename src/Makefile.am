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

noinst_LTLIBRARIES = libwbkinterpreter.la libwbkw32interpreter.la

w32bindkeys_SOURCES = main.c

libwbkinterpreter_la_SOURCES = logger.h   \
							   logger.c   \
							   util.h     \
							   util.c     \
							   kb.h       \
                               kb.c       \
	                           kbman.h    \
	                           kbman.c    \
	                           parser.h   \
	                           parser.c

libwbkinterpreter_la_CFLAGS = @collectionc_CFLAGS@
libwbkinterpreter_la_LDFLAGS = --static
libwbkinterpreter_la_LIBADD = @collectionc_LIBS@

libwbkw32interpreter_la_SOURCES = w32parser.h   \
	                           	  w32parser.c   \
	                           	  w32util.h     \
	                           	  w32util.c

libwbkw32interpreter_la_CFLAGS = @collectionc_CFLAGS@
libwbkw32interpreter_la_LDFLAGS = --static
libwbkw32interpreter_la_LIBADD = libwbkinterpreter.la @collectionc_LIBS@ #-mwindows

#w32bindkeys_CFLAGS = -static
w32bindkeys_LDFLAGS = --static
w32bindkeys_LDADD = libwbkw32interpreter.la


