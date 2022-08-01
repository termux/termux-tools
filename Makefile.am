# Copyright (C) 2022 Termux

# This file is part of termux-tools.

# termux-tools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# termux-tools is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with termux-tools.  If not, see
# <https://www.gnu.org/licenses/>.

SUBDIRS = scripts

# We have two variants of our motd message:
#   1. motd.sh: is sourced and echo'es a motd message with something that looks
#      like termux's logo.
#   3. motd-playstore: plain text that will only be displayed if the termux-app
#      version is very old (like the version found in playstore is)
sysconf_DATA = motd.sh motd-playstore

EXTRA_DIST = $(pkgdata_DATA)
