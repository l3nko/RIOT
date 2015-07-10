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

#modules for RPL
USEMODULE += defaulttransceiver
# USEMODULE += rpl

USEMODULE += sixlowpan
# USEMODULE += ieee802154
# USEMODULE += net_help
# USEMODULE += net_if


include $(RIOTBASE)/Makefile.include
