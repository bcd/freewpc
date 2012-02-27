
P := platform/p2k
$(eval $(call have,CONFIG_PLATFORM_P2K))

ifndef CONFIG_SIM
CPU := native
$(eval $(call have,CONFIG_CALLIO))
$(eval $(call have,CONFIG_PTHREADS))
include cpu/$(CPU)/Makefile
endif

#$(eval $(call have,CONFIG_VIDEO))
$(eval $(call have,CONFIG_AC))
$(eval $(call nohave,CONFIG_GI))

KERNEL_HW_OBJS += $(P)/main.o

