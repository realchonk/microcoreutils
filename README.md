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
| --host=HOST | host architecture |

### Building
Just a simple<br>
<code>make</code>

### Installation
NOTE: normally, you shouldn't install this package directly to your system.<br>
Use <code>make DESTDIR=... install</code>

## Programs
TODO: turn names into links to the corresponding files.

| Name | Is finished? | Has man page? | Notes |
|------|--------------|--------------|--------|
| basename | ✔ | ✔ | |
| cal | ✔ | ✔ | |
| cat | ✔ | ✔ | |
| chgrp | ✔ | ✔ | |
| chmod | ❌ | ❌ | Add support for u+g etc. |
| chown | ✔ | ✔ | |
| cksum | ✔ | ✔ | |
| clear | ✔ | ✔ | Check for portability |
| cp | ✔ | ✔ | |
| date | ✔ | ✔ | |
| dd | ❌ | ❌ | |
| dirname | ✔ | ✔ | |
| du | ❌ | ✔ | Missing options |
| echo | ✔ | ✔ | |
| ed | ❌ | ❌ | Only basic functionality |
| env | ✔ | ✔ | |
| expr | ✔ | ❌ | |
| false | ✔ | ✔ | Uses the same source-code as true |
| halt | ❌ | ❌ | Depends on init |
| head | ✔ | ✔ | |
| id | ✔ | ✔ | |
| init | ❌ | ❌ | |
| kill | ❌ | ✔ | |
| ln | ✔ | ✔ | |
| login | ❌ | ❌ | |
| ls | ❌ | ❌ | Missing options |
| mkdir | ✔ | ✔ | |
| mv | ✔ | ✔ | |
| pwd | ✔ |  ✔| |
| rm | ✔ | ✔ | |
| rmdir | ✔ | ✔ | |
| sleep | ✔ | ✔ | |
| sync | ✔ | ✔ | |
| tee | ✔ | ✔ | |
| test | ✔ | ❌ | |
| tr | ❌ | ✔ | Only basic matching |
| true | ✔ | ✔ | |
| tty | ✔ | ✔ | |
| uname | ✔ | ✔ | |
| unlink | ✔ | ✔ | |
| wc | ✔ | ✔ | |

## Notes
Finding sources with missing copyright:<br>
<code>diff \<(grep -rn '^//\s\*GNU General Public License' src | cut -d':' -f1) \<(find src -type f)</code>
