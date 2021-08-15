# Narrowly-POSIX complient UNIX tools
This project aims to be as POSIX complient as possible.<br>
There are no extra options than described by POSIX,<br>
but there are some extra programs (like init).

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
| --host=TARGET | host architecture |

### Building
Just a simple<br>
<code>make</code>

### Installation
NOTE: normally, you shouldn't install this package directly to your system.
<code>make DESTDIR=... install</code>

## Currently finished programs
- echo
- cat
- tee
- true
- false
- sleep
- unlink
- head
- mv
- rmdir
- basename
- dirname
- id
- uname
- wc
- rm
- date
- kill
- tty
- sync
- env
- pwd
- chown
- chgrp
- test
- ln
- cp

## Unfinished
- chmod (missing minor feature)
- ls (incomplete options)
- clear (unportable)
- ed (only basic functionality)
- halt
- init
- tr
- login (untested)
- du (missing -x option)
- dd (buggy)
