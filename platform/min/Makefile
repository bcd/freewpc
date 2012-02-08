
P := platform/min
$(eval $(call have,CONFIG_PLATFORM_MIN))

ifndef CPU
CPU := native
include cpu/$(CPU)/Makefile
$(eval $(call have,CONFIG_CALLIO))
endif

KERNEL_HW_OBJS += $(P)/main.o
KERNEL_HW_OBJS += $(P)/min-hw.o
