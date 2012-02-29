
P := platform/proc
$(eval $(call have,CONFIG_PLATFORM_PROC))

ifndef CONFIG_SIM
CPU := native
$(eval $(call have,CONFIG_CALLIO))
$(eval $(call have,CONFIG_PTHREADS))
$(eval $(call have,CONFIG_LINUX_GPIO))
$(eval $(call have,CONFIG_LINUX_INPUT))
include cpu/$(CPU)/Makefile
endif

KERNEL_HW_OBJS += $(P)/init-proc.o
KERNEL_HW_OBJS += $(P)/serial.o
KERNEL_HW_OBJS += $(P)/procdmd.o

