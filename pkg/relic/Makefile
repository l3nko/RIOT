RELIC_URL=http://github.com/relic-toolkit/relic.git
#RELIC_BRANCH=master

PKG_NAME=relic
PKG_URL=$(RELIC_URL)
#PKG_VERSION=$(RELIC_BRANCH)
PKG_DIR=$(CURDIR)/$(PKG_NAME)

ifneq ($(RIOTBOARD),)
include $(RIOTBOARD)/$(BOARD)/Makefile.include
endif

ifneq ($(RIOTBASE),)
INCLUDES += -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net/include \
-I$(RIOTBASE)/sys/posix/include -I$(RIOTBASE)/sys/posix/pnet/include
endif


all: build
	cp $(PKG_DIR)-build/lib/librelic_s.a $(BINDIR)$(PKG_NAME).a
	@echo RELIC OK

build:
	@echo "Building Relic ..."
	make -C $(PKG_DIR)-build
git:
	git clone "$(PKG_URL)" "$(PKG_NAME)-src";

clean:
	@echo "Removing file:"
	rm -rf $(BINDIR)$(PKG_NAME).a