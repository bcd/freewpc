
# CTEMP is the command that converts templates into C sources.
CTEMP=tools/ctemp -o $(BLDDIR)

DRIVER_OBJS :=

#
# A function for setting up rules on a per-class basis.
# $1 = name of the driver class
#
define driver-load
D_CLASS := $(strip $(1))
$(D_CLASS)_TEMPLATE := drivers/$(D_CLASS).ct
$(D_CLASS)_HEADER := $(BLDDIR)/$(D_CLASS).h
endef


#
# A function for creating a driver instance.
# $1 = name of the driver class
# $2 = instance number (zero-based)
# $3 = configuration parameters
#
define driver-open
D_CLASS := $(strip $(1))
D_INST := $(strip $(2))
D_ARGS := $(strip $(3))
D_SRC := $(BLDDIR)/$(D_CLASS)$(D_INST).c
D_OBJ := $(D_SRC:.c=.o)
D_HEADER := $(BLDDIR)/$(D_CLASS)$(D_INST).h

DRIVER_OBJS += $(D_CLASS), $(D_INST), $(D_ARGS) ;

$(D_OBJ) : $(D_SRC) $(D_HEADER) $($(D_CLASS)_HEADER)

$(D_SRC) $(D_HEADER) $($(D_CLASS)_HEADER) : $($(D_CLASS)_TEMPLATE)
endef

$(eval $(call driver-load,sling))
$(eval $(call driver-open,sling,1,bar))
$(eval $(call driver-open,sling,2,baz))
$(eval $(call driver-open,sling,0,foo))


#SLING_DRIVER=drivers/sling.ct
#SLING_CLASS=sling
#SLING_CLASS_HEADER=build/$(SLING_CLASS).h
#SLING_INST_HEADERS=build/$(SLING_CLASS)0.h build/$(SLING_CLASS)1.h
#SLING_SRCS=build/$(SLING_CLASS)0.c build/$(SLING_CLASS)1.c
#SLING_OBJS=$(SLING_SRCS:.c=.o)
#DRIVER_OBJS += $(SLING_OBJS)
#CONFIG_SLING_COUNT=2
#CONFIG_SLING_INSTANCE0=-D sw=SW_LEFT_SLING -D sol=SOL_LEFT_SLING
#CONFIG_SLING_INSTANCE1=-D sw=SW_RIGHT_SLING -D sol=SOL_RIGHT_SLING
#

drivers : $(DRIVER_OBJS)

clean-drivers :
	rm -f $(DRIVER_OBJS)

dump-drivers :
	@echo "DRIVER_OBJS = $(DRIVER_OBJS)"
	@echo "sling_TEMPLATE = $(sling_TEMPLATE)"

#build/sling0.o : build/sling0.c build/sling.h build/sling0.h
#build/sling1.o : build/sling0.c build/sling.h build/sling0.h
#
#build/sling0.c build/sling.h build/sling0.h : drivers/sling.ct
#	$(CTEMP) $(CONFIG_SLING_INSTANCE0) -i 0 drivers/sling.ct
#
#build/sling1.c build/sling1.h : drivers/sling.ct
#	$(CTEMP) $(CONFIG_SLING_INSTANCE1) -i 1 drivers/sling.ct

KERNEL_HW_OBJS += $(DRIVER_OBJS)
