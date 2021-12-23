# Narrowly-POSIX complient UNIX tools
This project aims to be as POSIX complient as possible.<br>
There are no extra options other than described by the POSIX standard,<br>
but there are a few extra programs (like init).

## Building/Installation
### Configuration
NOTE: if no configure script is available, please run
<code>./autogen.sh</code><br><br>
<code>./configure</code><br><br>
Common configure options:<br>
| Option | Description |
|--------|-------------|
| --help | see all available options |
| --prefix=PREFIX  | installation path |
| --host=HOST | host architecture |
| --enable-extra-progs | enable extra programs (like halt(8)) |

### Building
Just a simple<br>
<code>make</code>

### Installation
NOTE: normally, you shouldn't install this package directly to your system.<br>
Use <code>make DESTDIR=... install</code>

## Programs
| Name | Is finished? | Has man page? | Notes |
|------|--------------|--------------|--------|
| [basename](src/basename.c) | ✔ | [✔](https://stuerz.xyz/mc-basename.1.html) | |
| [cal](src/cal.c) | ✔ | [✔](https://stuerz.xyz/mc-cal.1.html) | |
| [cat](src/cat.c) | ✔ | [✔](https://stuerz.xyz/mc-cat.1.html) | |
| [chgrp](src/chgrp.c) | ✔ | [✔](https://stuerz.xyz/mc-chgrp.1.html) | |
| [chmod](src/chmod.c) | ❌ | [✔](https://stuerz.xyz/mc-chmod.1.html) | Add support for u+g etc. |
| [chown](src/chown.c) | ✔ | [✔](https://stuerz.xyz/mc-chown.1.html) | |
| [cksum](src/cksum.c) | ✔ | [✔](https://stuerz.xyz/mc-cksum.1.html) | |
| [cmp](src/cmp.c) | ✔ | [✔](https://stuerz.xyz/mc-cmp.1.html) | |
| [clear](src/clear.c) | ✔ | [✔](https://stuerz.xyz/mc-clear.1.html) | Check for portability |
| [cp](src/cp.c) | ❌ | [✔](https://stuerz.xyz/mc-cp.1.html) | Unimplemented options: -p |
| [date](src/date.c) | ✔ | [✔](https://stuerz.xyz/mc-date.1.html) | |
| [dd](src/dd.c) | ❌ | ❌ | |
| [dirname](src/dirname.c) | ✔ | [✔](https://stuerz.xyz/mc-dirname.1.html) | |
| [du](src/du.c) | ❌ | [✔](https://stuerz.xyz/mc-du.1.html) | Unimplemented options: -x |
| [echo](src/echo.c) | ✔ | [✔](https://stuerz.xyz/mc-echo.1.html) | |
| [ed](src/ed.c) | ❌ | ❌ | Only basic functionality |
| [env](src/env.c) | ✔ | [✔](https://stuerz.xyz/mc-env.1.html) | |
| [expr](src/expr.c) | ✔ | [✔](https://stuerz.xyz/mc-expr.1.html) | |
| [false](src/true.c) | ✔ | [✔](https://stuerz.xyz/mc-false.1.html) | Uses the same source-code as true |
| [grep](src/grep.c) | ❌ | [✔](https://stuerz.xyz/mc-grep.1.html) | Unimplemented options: -x |
| [halt](src/halt.c) | ❌ | [✔](https://stuerz.xyz/mc-halt.8.html) | Extra program, Depends on init |
| [head](src/head.c) | ✔ | [✔](https://stuerz.xyz/mc-head.1.html) | |
| [id](src/id.c) | ✔ | [✔](https://stuerz.xyz/mc-id.1.html) | |
| [init](src/init.c) | ❌ | [✔](https://stuerz.xyz/mc-init.8.html) | Extra program |
| [kill](src/kill.c) | ❌ | [✔](https://stuerz.xyz/mc-kill.1.html) | Doesn't support XSI syntax |
| [link](src/link.c) | ✔ | [✔](https://stuerz.xyz/mc-link.1.html) | |
| [ln](src/ln.c) | ✔ | [✔](https://stuerz.xyz/mc-ln.1.html) | |
| [logname](src/logname.c) | ✔ | [✔](https://stuerz.xyz/mc-logname.1.html) | |
| [login](src/login.c) | ❌ | ❌ | Extra program |
| [ls](src/ls.c) | ❌ | ❌ | Unimplemented options: -k, -q, -s, -g, -n, -o, -C, -m, -x, -F, -R, -d |
| [mkdir](src/mkdir.c) | ✔ | [✔](https://stuerz.xyz/mc-mkdir.1.html) | |
| [mkfifo](src/mkfifo.c) | ✔ | [✔](https://stuerz.xyz/mc-mkfifo.1.html) | |
| [mv](src/mv.c) | ✔ | [✔](https://stuerz.xyz/mc-mv.1.html) | |
| [nice](src/nice.c) | ✔ | [✔](https://stuerz.xyz/mc-nice.1.html) | |
| [pathchk](src/pathchk.c) | ✔ |  [✔](https://stuerz.xyz/mc-pathchk.1.html)| |
| [printf](src/printf.c) | ❌ |  ❌ | Incomplete format string |
| [pwd](src/pwd.c) | ✔ |  [✔](https://stuerz.xyz/mc-pwd.1.html)| |
| [renice](src/renice.c) | ✔ | [✔](https://stuerz.xyz/mc-renice.1.html) | |
| [rm](src/rm.c) | ✔ | [✔](https://stuerz.xyz/mc-rm.1.html) | |
| [rmdir](src/rmdir.c) | ✔ | [✔](https://stuerz.xyz/mc-rmdir.1.html) | |
| [sleep](src/sleep.c) | ✔ | [✔](https://stuerz.xyz/mc-sleep.1.html) | |
| [split](src/split.c) | ✔ | [✔](https://stuerz.xyz/mc-split.1.html) | |
| [sync](src/sync.c) | ✔ | [✔](https://stuerz.xyz/mc-sync.1.html) | Extra program |
| [tee](src/tee.c) | ✔ | [✔](https://stuerz.xyz/mc-tee.1.html) | |
| [test](src/test.c) | ✔ | [✔](https://stuerz.xyz/mc-test.1.html) | |
| [tr](src/tr.c) | ❌ | [✔](https://stuerz.xyz/mc-tr.1.html) | Only basic matching |
| [true](src/true.c) | ✔ | [✔](https://stuerz.xyz/mc-true.1.html) | |
| [tty](src/tty.c) | ✔ | [✔](https://stuerz.xyz/mc-tty.1.html) | |
| [uname](src/uname.c) | ✔ | [✔](https://stuerz.xyz/mc-uname.1.html) | |
| [unlink](src/unlink.c) | ✔ | [✔](https://stuerz.xyz/mc-unlink.1.html) | |
| [wc](src/wc.c) | ✔ | [✔](https://stuerz.xyz/mc-wc.1.html) | |
| [yes](src/yes.c) | ✔ | [✔](https://stuerz.xyz/mc-yes.1.html) | Extra program |

Note: if a program has a man page, the tick is a link to it.

## Notes
Finding sources with missing copyright:<br>
<code>diff \<(grep -rn '^//\s\*GNU General Public License' src | cut -d':' -f1) \<(find src -type f)</code><br>
Code I used to make the links:<br>
<code>awk -F'|' 'NR>=34&&NR<=74{printf "|[%s](src/%s.c)|%s|%s|%s|\n", $2, $2, $3, $4, $5 }' README.md| sed 's/\s\+/ /; s/\/\s*/\//; s/\s*\././; s/\[\s*/ [/; s/\s*\]/]/; s/)/) /'</code>
