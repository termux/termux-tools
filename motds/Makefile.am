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


# We have two variants of our motd message loaded by login script:
#   1. motd.sh: A dynamic motd that is executed with bash and echos a
#      motd message with escape sequences, optionally with a terminal logo.
#   2. motd: A static motd that is just read as is with cat and displayed.

# The motd-playstore is an additional motd that will only be displayed
# if the termux-app version is very old (like the version found on playstore)
sysconf_DATA = motd.sh motd-playstore

CLEANFILES = motd.sh


define process-rule
$1: $1.in Makefile
	@echo "Creating $1"
	@cat $(srcdir)/$1.in > $1
endef

$(eval $(call process-rule,motd.sh))
