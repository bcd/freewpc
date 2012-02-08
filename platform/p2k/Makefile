
P := platform/p2k
$(eval $(call have,CONFIG_PLATFORM_P2K))

ifndef CPU
CPU := native
include cpu/$(CPU)/Makefile
$(eval $(call have,CONFIG_CALLIO))
endif

#$(eval $(call have,CONFIG_VIDEO))
$(eval $(call have,CONFIG_AC))
$(eval $(call nohave,CONFIG_GI))

# Additional object files to be linked into the kernel region
KERNEL_HW_OBJS += $(P)/main.o

FIRST_BANK=0
