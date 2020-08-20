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
AM_CFLAGS = -g -O0
AM_LDFLAGS = -g -O0
else
AM_CFLAGS = -O2
AM_LDFLAGS = -O2
endif

if INSTALLLIBRARY
nobase_include_HEADERS = w32bindkeys/logger.h
nobase_include_HEADERS += w32bindkeys/util.h
nobase_include_HEADERS += w32bindkeys/be.h
nobase_include_HEADERS += w32bindkeys/b.h
nobase_include_HEADERS += w32bindkeys/kc.h
nobase_include_HEADERS += w32bindkeys/kc_sys.h
nobase_include_HEADERS += w32bindkeys/kbman.h
nobase_include_HEADERS += w32bindkeys/parser.h
nobase_include_HEADERS += w32bindkeys/kbdaemon.h
nobase_include_HEADERS += w32bindkeys/datafinder.h
endif

