
# Additional CFLAGS required on the WPC platform
CFLAGS += -DHAVE_FASTRAM_ATTRIBUTE -mdirect -DCONFIG_INSPECTOR -DCONFIG_PLATFORM_WPC -mwpc -fno-builtin -DCONFIG_PINMAME -DCONFIG_BIG_ENDIAN -mcode-section=sysrom -mdata-section=sysrom -mbss-section=ram 

ifdef CONFIG_16BIT
CFLAGS += -fshort-enums
else
CFLAGS += -mint8
endif

KERNEL_OBJS += platform/wpc/div10.o platform/wpc/task.o platform/wpc/vector.o

KERNEL_ASM_OBJS = platform/wpc/farcall.o platform/wpc/task_6809.o

