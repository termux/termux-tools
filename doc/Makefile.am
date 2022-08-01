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

man_MANS = termux.1

termux.1.md: ${srcdir}/termux.1.md.in
	@echo "Creating termux.1.md"
	@sed -e "s%[@]TERMUX_PREFIX[@]%$(termux_prefix)%g" \
	    -e "s%[@]TERMUX_HOME[@]%${termux_android_home}%g" \
	${srcdir}/termux.1.md.in > termux.1.md

termux.1: termux.1.md
	@echo "Generating termux.1"
	@pandoc --standalone --to man --output termux.1 termux.1.md
