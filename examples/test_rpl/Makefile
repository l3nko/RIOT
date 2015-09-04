# name of your application
APPLICATION = test_rpl

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += uart0
USEMODULE += shell
# USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += defaulttransceiver

USEMODULE += random

#modules for RPL
#USEMODULE += rpl
#USEMODULE += sixlowpan
#USEMODULE += ng_rpl
#USEMODULE += ieee802154
#USEMODULE += net_help
#USEMODULE += net_if
#USEMODULE += udp

#my port of RPL on NG stack
USEMODULE += ng_rpl

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += ng_netif_default
USEMODULE += auto_init_ng_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += ng_ipv6_default
USEMODULE += ng_udp
# This application dumps received packets to STDIO using the pktdump module
//USEMODULE += ng_pktdump
# Additional networking modules that can be dropped if not needed
USEMODULE += ng_icmpv6_echo




include $(RIOTBASE)/Makefile.include
