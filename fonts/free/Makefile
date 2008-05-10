TTFS := $(shell ls -1 *.ttf)
FONS := $(TTFS:.ttf=.fon)
TTF2FON := ../../tools/fontgen2
DEPS := Makefile $(TTF2FON)

all : $(FONS)

$(FONS) : %.fon : %.ttf $(DEPS)
	$(TTF2FON) $* > $@ ;\
	height=`grep "height [0-9]" $@ | awk '{print $$3}' | sort | tail -n 1` ;\
	sed -i -e "s/HEIGHT/$${height}/" $@

clean:
	rm -f $(FONS)
