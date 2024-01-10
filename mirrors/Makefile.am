# Default mirrors
pkgdata_MIRRORS = default

# Mirrors in Asia (excl. China and Russia)
pkgdata_ASIA_MIRRORS = mirror.albony.xyz mirror.bardia.tech	\
mirrors.cbrx.io linux.domainesia.com         \
mirror.nevacloud.com mirror.textcord.xyz

# Mirrors in China
pkgdata_CHINA_MIRRORS = mirrors.tuna.tsinghua.edu.cn		\
mirror.iscas.ac.cn mirrors.nju.edu.cn mirrors.pku.edu.cn	\
mirrors.ustc.edu.cn mirrors.hit.edu.cn mirrors.bfsu.edu.cn	\
mirrors.aliyun.com mirrors.cqupt.edu.cn mirrors.dgut.edu.cn	\
mirror.nyist.edu.cn mirrors.njupt.edu.cn mirrors.sau.edu.cn	\
mirrors.scau.edu.cn mirrors.sdu.edu.cn mirrors.sustech.edu.cn   \
mirrors.zju.edu.cn mirror.sjtu.edu.cn mirrors.qvq.net.cn

# Mirrors in Europe
pkgdata_EUROPE_MIRRORS = grimler.se mirror.termux.dev			\
packages.termux.dev termux.librehat.com mirror.mwt.me			\
termux.mentality.rip mirrors.sahilister.in termux.cdn.lumito.net	\
termux.astra.in.ua termux.3san.dev mirror.accum.se                      \
md.mirrors.hacktegic.com ftp.fau.de is.mirror.flokinet.net              \
ro.mirror.flokinet.net mirrors.medzik.dev mirrors.cfe.re                \
mirror.sunred.org mirror.autkin.net mirror.bouwhuis.network

# Mirrors in North America
pkgdata_NORTH_AMERICA_MIRRORS = plug-mirror.rcac.purdue.edu		\
dl.kcubeterm.com mirrors.utermux.dev mirror.fcix.net mirror.mwt.me	\
mirror.vern.cc mirror.csclub.uwaterloo.ca mirror.quantum5.ca

# Mirrors in South America
pkgdata_SOUTH_AMERICA_MIRRORS = mirrors.rda.run

# Mirrors in Russia
pkgdata_RUSSIA_MIRRORS = mirror.mephi.ru repository.su

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
$(eval $(call install-mirror-rule,china,CHINA))
$(eval $(call install-mirror-rule,europe,EUROPE))
$(eval $(call install-mirror-rule,north_america,NORTH_AMERICA))
$(eval $(call install-mirror-rule,south_america,SOUTH_AMERICA))
$(eval $(call install-mirror-rule,russia,RUSSIA))

$(eval $(call uninstall-mirror-rule,asia,ASIA))
$(eval $(call uninstall-mirror-rule,china,CHINA))
$(eval $(call uninstall-mirror-rule,europe,EUROPE))
$(eval $(call uninstall-mirror-rule,north_america,NORTH_AMERICA))
$(eval $(call uninstall-mirror-rule,south_america,SOUTH_AMERICA))
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

install-data-local: install-default install-asia install-china install-europe install-north_america install-south_america install-russia

uninstall-local: uninstall-default uninstall-asia uninstall-china uninstall-europe uninstall-north_america uninstall-south_america uninstall-russia

EXTRA_DIST = $(pkgdata_MIRRORS) $(pkgdata_ASIA_MIRRORS)	\
$(pkgdata_CHINA_MIRRORS) $(pkgdata_EUROPE_MIRRORS)	\
$(pkgdata_NORTH_AMERICA_MIRRORS) $(pkgdata_SOUTH_AMERICA_MIRRORS) 
