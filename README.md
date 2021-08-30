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

### Building
Just a simple<br>
<code>make</code>

### Installation
NOTE: normally, you shouldn't install this package directly to your system.<br>
Use <code>make DESTDIR=... install</code>

## Programs
TODO 2: add configure option to disable extra programs.<br>
TODO 3: move these 3 lines into a TODO file.<br>

| Name | Is finished? | Has man page? | Notes |
|------|--------------|--------------|--------|
| [basename](src/basename.1) s| ✔ | ✔ | |
| [cal](src/cal.1) s| ✔ | ✔ | |
| [cat](src/cat.1) s| ✔ | ✔ | |
| [chgrp](src/chgrp.1) s| ✔ | ✔ | |
| [chmod](src/chmod.1) s| ❌ | ❌ | Add support for u+g etc. |
| [chown](src/chown.1) s| ✔ | ✔ | |
| [cksum](src/cksum.1) s| ✔ | ✔ | |
| [clear](src/clear.1) s| ✔ | ✔ | Check for portability |
| [cp](src/cp.1) s| ✔ | ✔ | |
| [date](src/date.1) s| ✔ | ✔ | |
| [dd](src/dd.1) s| ❌ | ❌ | |
| [dirname](src/dirname.1) s| ✔ | ✔ | |
| [du](src/du.1) s| ❌ | ✔ | Missing options |
| [echo](src/echo.1) s| ✔ | ✔ | |
| [ed](src/ed.1) s| ❌ | ❌ | Only basic functionality |
| [env](src/env.1) s| ✔ | ✔ | |
| [expr](src/expr.1) s| ✔ | ❌ | |
| [false](src/false.1) s| ✔ | ✔ | Uses the same source-code as true |
| [halt](src/halt.1) s| ❌ | ❌ | Depends on init |
| [head](src/head.1) s| ✔ | ✔ | |
| [id](src/id.1) s| ✔ | ✔ | |
| [init](src/init.1) s| ❌ | ❌ | |
| [kill](src/kill.1) s| ❌ | ✔ | |
| [ln](src/ln.1) s| ✔ | ✔ | |
| [login](src/login.1) s| ❌ | ❌ | |
| [ls](src/ls.1) s| ❌ | ❌ | Missing options |
| [mkdir](src/mkdir.1) s| ✔ | ✔ | |
| [mv](src/mv.1) s| ✔ | ✔ | |
| [pwd](src/pwd.1) s| ✔ |  ✔| |
| [rm](src/rm.1) s| ✔ | ✔ | |
| [rmdir](src/rmdir.1) s| ✔ | ✔ | |
| [sleep](src/sleep.1) s| ✔ | ✔ | |
| [sync](src/sync.1) s| ✔ | ✔ | |
| [tee](src/tee.1) s| ✔ | ✔ | |
| [test](src/test.1) s| ✔ | ❌ | |
| [tr](src/tr.1) s| ❌ | ✔ | Only basic matching |
| [true](src/true.1) s| ✔ | ✔ | |
| [tty](src/tty.1) s| ✔ | ✔ | |
| [uname](src/uname.1) s| ✔ | ✔ | |
| [unlink](src/unlink.1) s| ✔ | ✔ | |
| [wc](src/wc.1) s| ✔ | ✔ | |

## Notes
Finding sources with missing copyright:<br>
<code>diff \<(grep -rn '^//\s\*GNU General Public License' src | cut -d':' -f1) \<(find src -type f)</code>
