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

bin_SCRIPTS = chsh dalvikvm login pkg su termux-backup			\
termux-change-repo termux-fix-shebang termux-info termux-open		\
termux-open-url termux-reload-settings termux-reset termux-restore	\
termux-setup-package-manager termux-setup-storage termux-wake-lock	\
termux-wake-unlock

CLEANFILES = $(bin_SCRIPTS)

do_subst = sed -e "s%[@]TERMUX_PREFIX[@]%$(termux_prefix)%g" \
	       -e "s%[@]TERMUX_APP_PACKAGE[@]%${termux_app_package}%g" \
	       -e "s%[@]TERMUX_BASE_DIR[@]%${termux_base_dir}%g" \
	       -e "s%[@]TERMUX_CACHE_DIR[@]%${termux_cache_dir}%g" \
	       -e "s%[@]TERMUX_HOME[@]%${termux_android_home}%g" \
	       -e "s%[@]PACKAGE_VERSION[@]%${PACKAGE_VERSION}%g" \
	       -e "s%[@]TERMUX_PACKAGE_FORMAT[@]%${termux_package_format}%g" \
	       -e "s%[@]TERMUX_PACKAGE_MANAGER[@]%${termux_package_manager}%g"

define tool-rule
$1: $1.in Makefile
	@echo "Creating $1"
	@$$(do_subst) < $(srcdir)/$1.in > $1
	@chmod u+x $1
endef

$(eval $(call tool-rule,chsh))
$(eval $(call tool-rule,dalvikvm))
$(eval $(call tool-rule,login))
$(eval $(call tool-rule,pkg))
$(eval $(call tool-rule,su))
$(eval $(call tool-rule,termux-backup))
$(eval $(call tool-rule,termux-change-repo))
$(eval $(call tool-rule,termux-fix-shebang))
$(eval $(call tool-rule,termux-info))
$(eval $(call tool-rule,termux-open))
$(eval $(call tool-rule,termux-open-url))
$(eval $(call tool-rule,termux-reload-settings))
$(eval $(call tool-rule,termux-reset))
$(eval $(call tool-rule,termux-restore))
$(eval $(call tool-rule,termux-setup-package-manager))
$(eval $(call tool-rule,termux-setup-storage))
$(eval $(call tool-rule,termux-wake-lock))
$(eval $(call tool-rule,termux-wake-unlock))
