#RELIC_URL=http://github.com/relic-toolkit/relic.git
#RELIC_BRANCH=master

PKG_NAME=relic
#PKG_URL=$(RELIC_URL)
#PKG_VERSION=$(RELIC_BRANCH)
PKG_DIR=$(CURDIR)/$(PKG_NAME)

ifneq ($(RIOTBOARD),)
include $(RIOTBOARD)/$(BOARD)/Makefile.include
endif

ifneq ($(RIOTBASE),)
INCLUDES += -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/net/include \
-I$(RIOTBASE)/sys/posix/include -I$(RIOTBASE)/sys/posix/pnet/include
endif


all:
	@echo "Coping file:" $(BINDIR)$(PKG_NAME).a
	cp $(PKG_DIR)-build/lib/librelic_s.a $(BINDIR)$(PKG_NAME).a
	@echo "RELIC OK"
