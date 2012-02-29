
ifndef CONFIG_GEN_RTT
SYSTEM_SCHEDULE := kernel/system.sched
SCHED_SRC := build/sched_irq.c
SCHED_OBJ := $(SCHED_SRC:.c=.o)
endif

# Basic kernel modules are generic and do not depend on
# the machine type at all.
KERNEL_BASIC_OBJS += $(if $(CONFIG_DMD), kernel/frame.o)
KERNEL_BASIC_OBJS += kernel/idle.o
KERNEL_BASIC_OBJS += kernel/lang.o
KERNEL_BASIC_OBJS += kernel/log.o
KERNEL_BASIC_OBJS +=	kernel/mbmode.o
KERNEL_BASIC_OBJS += kernel/misc.o
KERNEL_BASIC_OBJS += kernel/puts.o
KERNEL_BASIC_OBJS += kernel/random.o
KERNEL_BASIC_OBJS += kernel/sysinfo.o
KERNEL_BASIC_OBJS += kernel/timedmode.o

# Software kernel modules are hardware-agnostic, but may depend
# on machine flags and so need to be recompiled when the machine
# flags change.
KERNEL_SW_OBJS += kernel/deff.o
KERNEL_SW_OBJS += $(if $(CONFIG_FONT), kernel/font.o)
KERNEL_SW_OBJS += kernel/game.o
KERNEL_SW_OBJS += kernel/ladder.o
KERNEL_SW_OBJS += kernel/lamplist.o
KERNEL_SW_OBJS += kernel/lampset.o
KERNEL_SW_OBJS += kernel/player.o
KERNEL_SW_OBJS += kernel/printf.o
KERNEL_SW_OBJS += kernel/score.o
KERNEL_SW_OBJS += kernel/task.o

# Hardware kernel modules are incredibly hardware dependent,
# and are subject to change on different hardware platforms.
# This isn't really supported yet but it might be in the future
# (e.g. supporting System 11, or Whitestar, etc.)
# This group also includes any file that has a function called
# from interrupt context.
KERNEL_HW_OBJS += $(if $(CONFIG_AC), kernel/ac.o)
KERNEL_HW_OBJS += kernel/adj.o
KERNEL_HW_OBJS += kernel/audit.o
KERNEL_HW_OBJS += kernel/csum.o
KERNEL_HW_OBJS += $(if $(CONFIG_DMD),kernel/dmd.o)
KERNEL_HW_OBJS += kernel/error.o
KERNEL_HW_OBJS += kernel/file.o
KERNEL_HW_OBJS += kernel/flip.o
KERNEL_HW_OBJS += kernel/freetimer.o   # could be KERNEL_SW_OBJS
                                       # except for RTT.
KERNEL_HW_OBJS += kernel/init.o
KERNEL_HW_OBJS += kernel/lamp.o
KERNEL_HW_OBJS += kernel/leff.o   # why not KERNEL_SW_OBJS?
KERNEL_HW_OBJS += $(if $(CONFIG_DMD_OR_ALPHA), kernel/message.o)
KERNEL_HW_OBJS += $(if $(CONFIG_ALPHA),kernel/segment.o)
KERNEL_HW_OBJS += kernel/sol.o
KERNEL_HW_OBJS += kernel/sound.o
KERNEL_HW_OBJS += kernel/switches.o
KERNEL_HW_OBJS += kernel/timer.o   # why not KERNEL_SW_OBJS?
KERNEL_HW_OBJS += $(if $(CONFIG_GI), kernel/triac.o)

KERNEL_OBJS += $(KERNEL_BASIC_OBJS) $(KERNEL_SW_OBJS) $(KERNEL_HW_OBJS)

