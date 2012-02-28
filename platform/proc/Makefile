
N := platform/native
P := platform/proc
NATIVE_OBJS :=

# Common native mode CPU configuration
PLATFORM := proc
CPU := native
CONFIG_PTH := y
CONFIG_SOFT_REALTIME := y

include cpu/$(CPU)/Makefile

$(eval $(call have,CONFIG_PLATFORM_PROC))
$(eval $(call have,CONFIG_DMD))
$(eval $(call have,CONFIG_DMD_STUB))
$(eval $(call have,CONFIG_FONT))
$(eval $(call have,CONFIG_ANIMATION))
$(eval $(call have,CONFIG_REMOTEIO))


# Platform dependent files
KERNEL_HW_OBJS += $(P)/init-proc.o
KERNEL_HW_OBJS += $(P)/serial.o
KERNEL_HW_OBJS += $(P)/procdmd.o
NATIVE_OBJS += $(N)/bcd_string.o

# Additional defines
CFLAGS += -I$(P)

CFLAGS += -DMACHINE_SHORTNAME=\"$(MACHINE)\"

# Import images that are common to all games.
IMAGE_MAP += $(P)/../wpc/wpc.ild

FIRST_BANK=0

