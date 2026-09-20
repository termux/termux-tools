# termux-tools

Some scripts and small programs that are packaged into termux's
termux-tools package.

## Mirrors

The mirrors/ directory contains files with information about all the termux mirrors, group by different parts of the world. To add a new mirror, open a pull request to add a new file in the appropriate directory.

The file should be named after the base url of the repository, for example packages.termux.dev, and the content as follows:

```
# This file is sourced by pkg
# Mirror by <username or short name of mirror owner>. Hosted in <city>, <country>.
# <username longer name of mirror owner> : <url with more information about mirror or its owner>
# <Longer description, for example detailing location, sync period, mirror bandwidth, ipv6 capability and other relevant info. Can be multiple sentences.>
WEIGHT=1
MAIN="<url to termux-main repo>"
ROOT="<url to termux-root repo>"
X11="<url to termux-x11 repo>"
```

where text within <> should be replaced. A fully filled in example is:

```
# This file is sourced by pkg
# Mirror by ACC. Hosted in Umeå, Sweden.
# Academic Computer Club in Umeå | https://accum.se
# Hosted in Umeå, Sweden. Updated every four hours.
WEIGHT=1
MAIN="https://mirror.accum.se/mirror/termux.dev/termux-main"
ROOT="https://mirror.accum.se/mirror/termux.dev/termux-root"
X11="https://mirror.accum.se/mirror/termux.dev/termux-x11"
```

The commented header lines are used as follows:

* First line `# This file is sourced by pkg` is just for general information and not used for anything.  
* Second line is used verbatim in pkg and presented to user as mirror description. Should hence be kept short.  
* Third and fourth lines contains the name, url and general information that will be printed in generated wiki page decribing all mirrors.  

