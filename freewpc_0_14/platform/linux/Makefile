
# Additional CFLAGS required on the Linux platform
CFLAGS += -O -I`pth-config --cflags` -DCONFIG_PLATFORM_LINUX -g -DCONFIG_LITTLE_ENDIAN -pg
HOST_LFLAGS += -pg
HOST_LIBS += -lpth

# Additional object files to be linked into the kernel region
KERNEL_OBJS += kernel/linux.o platform/linux/task.o
