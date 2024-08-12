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

SUBDIRS = . scripts doc mirrors motds src

CONFFILES = \
etc/motd \
etc/motd.sh \
etc/motd-playstore \
etc/profile.d/init-termux-properties.sh \
etc/termux-login.sh

do_subst = sed -e "s%[@]TERMUX_PREFIX[@]%$(termux_prefix)%g" \
	       -e "s%[@]TERMUX_APP_PACKAGE[@]%${termux_app_package}%g" \
	       -e "s%[@]TERMUX_BASE_DIR[@]%${termux_base_dir}%g" \
	       -e "s%[@]TERMUX_CACHE_DIR[@]%${termux_cache_dir}%g" \
	       -e "s%[@]TERMUX_HOME[@]%${termux_android_home}%g" \
	       -e "s%[@]PACKAGE_VERSION[@]%${PACKAGE_VERSION}%g" \
	       -e "s%[@]TERMUX_PACKAGE_FORMAT[@]%${TERMUX_PACKAGE_FORMAT}%g" \
	       -e "s%[@]TERMUX_PACKAGE_MANAGER[@]%${TERMUX_PACKAGE_MANAGER}%g"

define sed-rule
$1: $1.in Makefile
	@echo "Creating $1"
	@$$(do_subst) < $(srcdir)/$1.in > $1
endef

# Login script
sysconf_DATA = termux-login.sh

# profile.d script
pkgdata_PROFILE = init-termux-properties.sh

pkgdata_EXAMPLES = termux.properties

$(eval $(call sed-rule,init-termux-properties.sh))

$(eval $(call sed-rule,termux-login.sh))



create-deb-control-files:
	printf "" > conffiles
	for f in $(CONFFILES); do \
		printf "%s\n" "$$f" >> conffiles; \
	done
	printf "#!%s/bin/bash\n\n" "${termux_prefix}" > preinst



install-data-local: create-deb-control-files $(pkgdata_PROFILE)
	$(MKDIR_P) $(DESTDIR)$(datarootdir)/examples/termux
	for f in $(pkgdata_EXAMPLES); do \
		$(INSTALL) -m 644 $(srcdir)/$$f \
		$(DESTDIR)$(datarootdir)/examples/termux; \
	done
	$(MKDIR_P) $(DESTDIR)$(sysconfdir)/profile.d
	for f in $(pkgdata_PROFILE); do \
		$(INSTALL) -m 644 $$f \
		$(DESTDIR)$(sysconfdir)/profile.d; \
	done

uninstall-local:
	for f in $(pkgdata_EXAMPLES); do \
		rm -f $(DESTDIR)$(datarootdir)/examples/termux/$$f; \
	done
	-rmdir $(DESTDIR)$(datarootdir)/examples/termux
	for f in $(pkgdata_PROFILE); do \
		rm -f $(DESTDIR)$(sysconfdir)/profile.d/$$f; \
	done
	-rmdir $(DESTDIR)$(sysconfdir)/profile.d

EXTRA_DIST = $(pkgdata_DATA) $(pkgdata_PROFILE) $(pkgdata_EXAMPLES)
