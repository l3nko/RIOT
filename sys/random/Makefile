#MERSENNE_MODE	0
#TINY_MT_MODE 	1

MODE := 1

# If a valid RPL_MOP was provided, use it for RPL
ifneq (,$(MODE))
	CFLAGS += -DRANDOM_MODE=$(MODE)
endif

SRC = random.c
ifeq (1,$(MODE))
	SRC += tinymt32.c
endif
ifeq (0,$(MODE))
	SRC += mersenne.c
endif

include $(RIOTBASE)/Makefile.base
