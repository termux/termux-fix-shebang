# termux-fix-shebang

Small program to modify the shebang of scripts to work in a Termux environment. Typically `#!/bin/sh` is replaced with `#!/data/data/com.termux/files/usr/bin/sh`.

## What is a shebang?

A way to tell the shell how to run a script. See the Wikipedia article for more information: [Shebang (Unix)](https://en.wikipedia.org/wiki/Shebang_%28Unix%29).

## Why is termux-fix-shebang needed?

Termux does not follow the [Filesystem Hierarchy Standard](https://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard), due to how Android works. Pretty much all other Linux distributions does follow the standard, and hence most scripts come with a hardcoded shebang like `#!/bin/sh` or `/usr/bin/env python3`. For the script to fully work Termux 

See also [Termux wiki: Differences from Linux](https://wiki.termux.com/wiki/Differences_from_Linux).

## In my tests I am able to run a script with #!/bin/sh without issues

Termux also comes with a LD_PRELOAD'ed [library termux-exec](https://github.com/termux/termux-exec) which automatically translates the interpreter path before it is executed, so under many circumstances scripts with non-functional shebang does work. However, when running scripts in the addon apps (like termux-widget or termux-tasker), and under other edge-cases, this library is not loaded and the shebang needs to point to a proper program.
