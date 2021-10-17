#  Copyright (C) 2021 Benjamin Stürz
#  
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.


bin_PROGRAMS = basename 		\
					cal				\
					cat				\
					chgrp				\
					chmod				\
					chown				\
					cksum				\
					cmp				\
					clear				\
					cp					\
					date				\
					dd					\
					dirname			\
					du					\
					echo				\
					ed					\
					env				\
					expr				\
					false				\
					head				\
					id					\
					kill				\
					link				\
					ln					\
					logname			\
					ls					\
					mkdir				\
					mv					\
					nice				\
					pathchk			\
					pwd				\
					renice			\
					rm					\
					rmdir				\
					sleep				\
					tee				\
					test				\
					tr					\
					true				\
					uname				\
					unlink			\
					wc

sbin_PROGRAMS =

man1_MANS = doc/basename.1		\
				doc/cal.1			\
				doc/cat.1			\
				doc/chgrp.1			\
				doc/chmod.1			\
				doc/chown.1			\
				doc/cksum.1			\
				doc/cmp.1			\
				doc/clear.1			\
				doc/cp.1				\
				doc/date.1			\
				doc/dirname.1		\
				doc/du.1				\
				doc/echo.1			\
				doc/env.1			\
				doc/expr.1			\
				doc/false.1			\
				doc/head.1			\
				doc/id.1				\
				doc/kill.1			\
				doc/link.1			\
				doc/ln.1				\
				doc/logname.1		\
				doc/mkdir.1			\
				doc/mv.1				\
				doc/nice.1			\
				doc/pathchk.1		\
				doc/pwd.1			\
				doc/renice.1		\
				doc/rm.1				\
				doc/rmdir.1			\
				doc/sleep.1			\
				doc/tee.1			\
				doc/test.1			\
				doc/tr.1				\
				doc/true.1			\
				doc/tty.1			\
				doc/uname.1			\
				doc/unlink.1		\
				doc/wc.1
man8_MANS =

if ENABLE_EXTRA_PROGS
bin_PROGRAMS	+= login sync
sbin_PROGRAMS 	+= halt init
man1_MANS		+= doc/sync.1
man8_MANS 		+= doc/halt.8 doc/init.8
endif

basename_SOURCES	= src/basename.c
cal_SOURCES			= src/cal.c
cat_SOURCES			= src/cat.c
chgrp_SOURCES		= src/chgrp.c
chmod_SOURCES		= src/chmod.c
chown_SOURCES		= src/chown.c
cksum_SOURCES		= src/cksum.c
cmp_SOURCES			= src/cmp.c
clear_SOURCES		= src/clear.c
cp_SOURCES			= src/cp.c
date_SOURCES		= src/date.c
dd_SOURCES			= src/dd.c
dirname_SOURCES	= src/dirname.c
du_SOURCES			= src/du.c
echo_SOURCES		= src/echo.c
ed_SOURCES			= src/ed.c
env_SOURCES			= src/env.c
expr_SOURCES		= src/expr.c
halt_SOURCES		= src/halt.c
false_SOURCES		= src/true.c
head_SOURCES		= src/head.c
id_SOURCES			= src/id.c
init_SOURCES 		= src/init.c
kill_SOURCES		= src/kill.c
link_SOURCES		= src/link.c
ln_SOURCES			= src/ln.c
logname_SOURCES	= src/logname.c
login_SOURCES		= src/login.c
ls_SOURCES			= src/ls.c
mkdir_SOURCES		= src/mkdir.c
mv_SOURCES			= src/mv.c
pathchk_SOURCES	= src/pathchk.c
pwd_SOURCES			= src/pwd.c
rm_SOURCES			= src/rm.c
rmdir_SOURCES		= src/rmdir.c
nice_SOURCES		= src/nice.c
renice_SOURCES		= src/renice.c
sleep_SOURCES		= src/sleep.c
sync_SOURCES		= src/sync.c
tee_SOURCES			= src/tee.c
test_SOURCES		= src/test.c
tr_SOURCES			= src/tr.c
true_SOURCES		= src/true.c
tty_SOURCES			= src/tty.c
uname_SOURCES		= src/uname.c
unlink_SOURCES		= src/unlink.c
wc_SOURCES			= src/wc.c

login_LDFLAGS		= -lcrypt

true_CPPFLAGS		= -DEXIT_CODE=0
false_CPPFLAGS		= -DEXIT_CODE=1

AM_CPPFLAGS			= -Iinclude

install-data-local:
	mkdir -p $(DESTDIR)/usr/bin
	ln -sf test $(DESTDIR)/$(bindir)/[
	ln -sf ../../$(bindir)/env $(DESTDIR)/usr/bin/env

uninstall-local:
	rm -f $(DESTDIR)/$(bindir)/[

full-clean: clean
	rm -f Makefile Makefile.in configure configure~ src/.dirstamp stamp-h1 aclocal.m4
	rm -f config.h config.h.in config.h.in~ config.log config.status
	rm -rf autom4te.cache build-aux src/.deps .deps

