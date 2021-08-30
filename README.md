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
| [basename](src/basename.c) | ✔ | ✔ | |
| [cal](src/cal.c) | ✔ | ✔ | |
| [cat](src/cat.c) | ✔ | ✔ | |
| [chgrp](src/chgrp.c) | ✔ | ✔ | |
| [chmod](src/chmod.c) | ❌ | ❌ | Add support for u+g etc. |
| [chown](src/chown.c) | ✔ | ✔ | |
| [cksum](src/cksum.c) | ✔ | ✔ | |
| [clear](src/clear.c) | ✔ | ✔ | Check for portability |
| [cp](src/cp.c) | ✔ | ✔ | |
| [date](src/date.c) | ✔ | ✔ | |
| [dd](src/dd.c) | ❌ | ❌ | |
| [dirname](src/dirname.c) | ✔ | ✔ | |
| [du](src/du.c) | ❌ | ✔ | Missing options |
| [echo](src/echo.c) | ✔ | ✔ | |
| [ed](src/ed.c) | ❌ | ❌ | Only basic functionality |
| [env](src/env.c) | ✔ | ✔ | |
| [expr](src/expr.c) | ✔ | ❌ | |
| [false](src/true.c) | ✔ | ✔ | Uses the same source-code as true |
| [halt](src/halt.c) | ❌ | ❌ | Depends on init |
| [head](src/head.c) | ✔ | ✔ | |
| [id](src/id.c) | ✔ | ✔ | |
| [init](src/init.c) | ❌ | ❌ | |
| [kill](src/kill.c) | ❌ | ✔ | |
| [ln](src/ln.c) | ✔ | ✔ | |
| [login](src/login.c) | ❌ | ❌ | |
| [ls](src/ls.c) | ❌ | ❌ | Missing options |
| [mkdir](src/mkdir.c) | ✔ | ✔ | |
| [mv](src/mv.c) | ✔ | ✔ | |
| [pwd](src/pwd.c) | ✔ |  ✔| |
| [rm](src/rm.c) | ✔ | ✔ | |
| [rmdir](src/rmdir.c) | ✔ | ✔ | |
| [sleep](src/sleep.c) | ✔ | ✔ | |
| [sync](src/sync.c) | ✔ | ✔ | |
| [tee](src/tee.c) | ✔ | ✔ | |
| [test](src/test.c) | ✔ | ❌ | |
| [tr](src/tr.c) | ❌ | ✔ | Only basic matching |
| [true](src/true.c) | ✔ | ✔ | |
| [tty](src/tty.c) | ✔ | ✔ | |
| [uname](src/uname.c) | ✔ | ✔ | |
| [unlink](src/unlink.c) | ✔ | ✔ | |
| [wc](src/wc.c) | ✔ | ✔ | |

## Notes
Finding sources with missing copyright:<br>
<code>diff \<(grep -rn '^//\s\*GNU General Public License' src | cut -d':' -f1) \<(find src -type f)</code><br>
Code I used to make the links:<br>
<code>awk -F'|' 'NR>=34&&NR<=74{printf "|[%s](src/%s.c)|%s|%s|%s|\n", $2, $2, $3, $4, $5 }' README.md| sed 's/\s\+/ /; s/\/\s*/\//; s/\s*\././; s/\[\s*/ [/; s/\s*\]/]/; s/)/) /'</code>
