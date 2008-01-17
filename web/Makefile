MAJOR = 0
MINOR = 16
GAMES = afm tz wcs t2
ROMDIR = /home/bcd/oddchange/freewpc/freewpc.git/web/v2/build/freewpc/releases

export NOCONFIG=y
export FREEWPC_DEBUGGER=n
export PLATFORM=wpc
export GCC_VERSION=4.3.0
export SYSTEM_MAJOR=0
export SYSTEM_MINOR=16
export PINMAME=/home/bcd/src/pinmame/xpinmamed.x11
export TARGET_ROMPATH=/home/bcd/roms/pinmame
export BG=

all : tarball roms

tarball:
	@true $(foreach game,$(GAMES), && cd .. && $(MAKE) clean MACHINE=$(game) && cd -)
	@cd .. && tar zcvf freewpc-$(MAJOR).$(MINOR).tar.gz \
		--exclude=doxygen --exclude=web --exclude=doc * && \
	mv freewpc-$(MAJOR).$(MINOR).tar.gz $(ROMDIR)

roms:
	true $(foreach game,$(GAMES), && cd .. \
		&& $(MAKE) clean MACHINE=$(game) \
		&& $(MAKE) MACHINE=$(game) \
		&& $(MAKE) run MACHINE=$(game) \
		&& zip -9 f$(game)$(MAJOR)_$(MINOR).zip f$(game)$(MAJOR)_$(MINOR).rom \
		&& cp -p f$(game)$(MAJOR)_$(MINOR).zip $(ROMDIR) \
		&& cd -)

