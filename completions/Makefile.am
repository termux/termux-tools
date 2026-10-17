bashcompletiondir = $(datarootdir)/bash-completion/completions
bashcompletion_DATA = \
	bash/chsh \
	bash/pkg \
	bash/termux-backup \
	bash/termux-info \
	bash/termux-open \
	bash/termux-restore

zshcompletiondir = $(datarootdir)/zsh/site-functions
zshcompletion_DATA = \
	zsh/_chsh \
	zsh/_pkg \
	zsh/_termux-backup \
	zsh/_termux-info \
	zsh/_termux-open \
	zsh/_termux-restore

fishcompletiondir = $(datarootdir)/fish/vendor_completions.d
fishcompletion_DATA = \
	fish/chsh.fish \
	fish/pkg.fish \
	fish/termux-backup.fish \
	fish/termux-info.fish \
	fish/termux-open.fish \
	fish/termux-restore.fish

CLEANFILES = \
	bash/pkg \
	fish/pkg.fish \
	zsh/_pkg

do_subst = sed -e "s%[@]TERMUX_PREFIX[@]%$(termux_prefix)%g"

define completion-rule
$1: $1.in Makefile
	@echo "Creating $1"
	@$$(MKDIR_P) $$(@D)
	@$$(do_subst) < $$(srcdir)/$1.in > $$@
endef

$(eval $(call completion-rule,bash/pkg))
$(eval $(call completion-rule,fish/pkg.fish))
$(eval $(call completion-rule,zsh/_pkg))

install-data-hook:
	@$(LN_S) -f termux-open $(DESTDIR)$(bashcompletiondir)/xdg-open
	@$(LN_S) -f _termux-open $(DESTDIR)$(zshcompletiondir)/_xdg-open
	@$(LN_S) -f termux-open.fish $(DESTDIR)$(fishcompletiondir)/xdg-open.fish

uninstall-hook:
	@rm -f $(DESTDIR)$(bashcompletiondir)/xdg-open
	@rm -f $(DESTDIR)$(zshcompletiondir)/_xdg-open
	@rm -f $(DESTDIR)$(fishcompletiondir)/xdg-open.fish

EXTRA_DIST = \
	bash/chsh \
	bash/pkg.in \
	bash/termux-backup \
	bash/termux-info \
	bash/termux-open \
	bash/termux-restore \
	fish/chsh.fish \
	fish/pkg.fish.in \
	fish/termux-backup.fish \
	fish/termux-info.fish \
	fish/termux-open.fish \
	fish/termux-restore.fish \
	zsh/_chsh \
	zsh/_pkg.in \
	zsh/_termux-backup \
	zsh/_termux-info \
	zsh/_termux-open \
	zsh/_termux-restore
