# Default mirrors
pkgdata_MIRRORS = default

# Mirrors in Asia (excl. Chinese_Mainland and Russia)
pkgdata_ASIA_MIRRORS = mirror.albony.in mirror.bardia.tech          \
mirrors.cbrx.io linux.domainesia.com mirror.nevacloud.com            \
mirror.textcord.xyz tmx.xvx.my.id  mirrors.nguyenhoang.cloud         \
mirror.freedif.org mirror.twds.com.tw mirrors.in.sahilister.net       \
mirrors.saswata.cc mirrors.omsinchan.ac.th termux.niranjan.co

# Mirrors in Chinese Mainland
pkgdata_CHINESE_MAINLAND_MIRRORS = mirrors.tuna.tsinghua.edu.cn                 \
mirror.iscas.ac.cn mirrors.nju.edu.cn mirrors.pku.edu.cn             \
mirrors.ustc.edu.cn mirrors.bfsu.edu.cn	mirrors.aliyun.com           \
mirrors.cqupt.edu.cn mirror.nyist.edu.cn mirrors.sau.edu.cn	         \
mirrors.sdu.edu.cn mirrors.sustech.edu.cn mirrors.zju.edu.cn         \
mirror.sjtu.edu.cn mirrors.qvq.net.cn mirrors.hust.edu.cn

# Mirrors in Europe
pkgdata_EUROPE_MIRRORS = grimler.se mirror.termux.dev			\
packages.termux.dev termux.librehat.com mirror.mwt.me			\
termux.mentality.rip mirrors.de.sahilister.net termux.cdn.lumito.net	\
termux.3san.dev mirror.accum.se			\
md.mirrors.hacktegic.com ftp.fau.de is.mirror.flokinet.net		\
ro.mirror.flokinet.net mirrors.medzik.dev mirrors.cfe.re		\
mirror.sunred.org mirror.autkin.net mirror.bouwhuis.network		\
mirror.leitecastro.com ftp.agdsn.de mirror.polido.pt			\
nl.mirror.flokinet.net

# Mirrors in North America
pkgdata_NORTH_AMERICA_MIRRORS = plug-mirror.rcac.purdue.edu          \
dl.kcubeterm.com mirrors.utermux.dev mirror.fcix.net mirror.mwt.me   \
mirror.vern.cc mirror.csclub.uwaterloo.ca mirror.quantum5.ca		 \
termux.danyael.xyz gnlug.org

# Mirrors in Oceania
pkgdata_OCEANIA_MIRRORS = mirrors.middlendian.com

# Mirrors in Russia
pkgdata_RUSSIA_MIRRORS = mirror.mephi.ru repository.su


# Removed, renamed or moved mirrors
# Mirrors files are marked as conffiles and they are manually deleted
# in preinst step of package upgrade as conffiles removed in a newer
# version of the package are only deleted if package is purged and
# would otherwise be left as is after package upgrades and would still
# get used during mirror selection.
pkgdata_MIRRORS_REMOVED = \
asia/packages.nscdn.top \
china/mirrors.dgut.edu.cn \
china/mirrors.hit.edu.cn \
china/mirrors.njupt.edu.cn \
china/mirrors.scau.edu.cn \
china/mirror.iscas.ac.cn \
china/mirror.nyist.edu.cn \
china/mirror.sjtu.edu.cn \
china/mirrors.aliyun.com \
china/mirrors.bfsu.edu.cn \
china/mirrors.cqupt.edu.cn \
china/mirrors.nju.edu.cn \
china/mirrors.pku.edu.cn \
china/mirrors.qvq.net.cn \
china/mirrors.sau.edu.cn \
china/mirrors.sdu.edu.cn \
china/mirrors.sustech.edu.cn \
china/mirrors.tuna.tsinghua.edu.cn \
china/mirrors.ustc.edu.cn \
china/mirrors.zju.edu.cn \
europe/cdn.lumito.net \
europe/mirror.termux.dv \
europe/termux.sahilister.in \
europe/termux.astra.in.ua \
north_america/packages.termux.dev \
oceania/mirrors.wale.id.au \
russia/mirror.surf \
south_america/mirrors.rda.run



define install-mirror-rule
install-$1:
	$$(MKDIR_P) $$(DESTDIR)$$(sysconfdir)/termux/mirrors/$1
	for f in $$(pkgdata_$2_MIRRORS); do \
		$(INSTALL) -m 644 $(srcdir)/$1/$$$$f \
		$(DESTDIR)$(sysconfdir)/termux/mirrors/$1; \
	done
endef

define uninstall-mirror-rule
uninstall-$1:
	for f in $$(pkgdata_$2_MIRRORS); do \
		rm -f $(DESTDIR)$(sysconfdir)/termux/mirrors/$1/$$$$f; \
	done
	-rmdir $(DESTDIR)$(sysconfdir)/termux/mirrors/$1
endef

$(eval $(call install-mirror-rule,asia,ASIA))
$(eval $(call install-mirror-rule,chinese_mainland,CHINESE_MAINLAND))
$(eval $(call install-mirror-rule,europe,EUROPE))
$(eval $(call install-mirror-rule,north_america,NORTH_AMERICA))
$(eval $(call install-mirror-rule,oceania,OCEANIA))
$(eval $(call install-mirror-rule,russia,RUSSIA))

$(eval $(call uninstall-mirror-rule,asia,ASIA))
$(eval $(call uninstall-mirror-rule,chinese_mainland,CHINESE_MAINLAND))
$(eval $(call uninstall-mirror-rule,europe,EUROPE))
$(eval $(call uninstall-mirror-rule,north_america,NORTH_AMERICA))
$(eval $(call uninstall-mirror-rule,oceania,OCEANIA))
$(eval $(call uninstall-mirror-rule,russia,RUSSIA))



install-default: $(pkgdata_MIRRORS)
	$(MKDIR_P) $(DESTDIR)$(sysconfdir)/termux/mirrors
	for f in $(pkgdata_MIRRORS); do \
		$(INSTALL) -m 644 $(srcdir)/$$f \
		$(DESTDIR)$(sysconfdir)/termux/mirrors; \
	done

uninstall-default:
	for f in $(pkgdata_MIRRORS); do \
		rm -f $(DESTDIR)$(sysconfdir)/termux/mirrors/$$f; \
	done
	-rmdir $(DESTDIR)$(sysconfdir)/termux/mirrors



uninstall-removed-mirrors:
	for f in $(pkgdata_MIRRORS_REMOVED); do \
		rm -f $(DESTDIR)$(sysconfdir)/termux/mirrors/$$f; \
	done



create-deb-control-files:
	sh -c "cd $$(dirname $(DESTDIR)$(sysconfdir)); find $$(basename $(DESTDIR)$(sysconfdir))/termux/mirrors -type f" >> ../conffiles;
	printf "# Delete removed mirrors\nfor f in %s; do\n    if test -f \"%s/etc/termux/mirrors/\$$f\"; then\n        echo \"Deleting removed mirror: \$$f\"; rm -f \"%s/etc/termux/mirrors/\$$f\";\n    fi\ndone" "$(pkgdata_MIRRORS_REMOVED)" "$(termux_prefix)" "$(termux_prefix)" >> ../preinst;



install-data-local: uninstall-removed-mirrors install-default install-asia install-chinese_mainland install-europe install-north_america install-oceania install-russia create-deb-control-files

uninstall-local: uninstall-removed-mirrors uninstall-default uninstall-asia uninstall-chinese_mainland uninstall-europe uninstall-north_america uninstall-oceania uninstall-russia

EXTRA_DIST = $(pkgdata_MIRRORS) $(pkgdata_ASIA_MIRRORS)		\
$(pkgdata_CHINESE_MAINLAND_MIRRORS) $(pkgdata_EUROPE_MIRRORS)		\
$(pkgdata_NORTH_AMERICA_MIRRORS) $(pkgdata_OCEANIA_MIRRORS)	
