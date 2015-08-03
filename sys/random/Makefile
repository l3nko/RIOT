#MERSENNE_MODE	0
#TINY_MT_MODE 	1

MODE := 1

# If a valid RPL_MOP was provided, use it for RPL
ifneq (,$(MODE))
	CFLAGS += -DRANDOM_MODE=$(MODE)
endif


include $(RIOTBASE)/Makefile.base
