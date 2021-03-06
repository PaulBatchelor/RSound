include config.mk

uname := $(shell uname -a)
ifneq ($(findstring Darwin,$(uname)),)
   PLATFORM := OSX
else
   PLATFORM := X
endif

TARGET_SERVER = rsd
TARGET_CLIENT = rsdplay
ifeq ($(PLATFORM),OSX)
   TARGET_LIB = librsound/librsound.dylib
else
   TARGET_LIB = librsound/librsound.so.3.0.0
endif
TARGET_LIB_OBJ = librsound/librsound.o librsound/buffer.o
TARGET_LIB_OBJ_STATIC = librsound/librsound.a

PKGCONF_PATH = $(PREFIX)/lib/pkgconfig/rsound.pc

TARGET_SERVER_LIBS = -lpthread
TARGET_LIB_LIBS = -lpthread
OPT_SERV_LIBS =
OPT_SERV_OBJ =

ifeq ($(HAVE_ALSA), 1)
   OPT_SERV_LIBS += -lasound
   OPT_SERV_OBJ += drivers/alsa.o
endif

ifeq ($(HAVE_OSS), 1)
ifeq ($(NEED_OSSAUDIO), 1)
   OPT_SERV_LIBS += -lossaudio
endif
   OPT_SERV_OBJ += drivers/oss.o
endif

ifeq ($(HAVE_LIBAO), 1)
   OPT_SERV_LIBS += -lao
   OPT_SERV_OBJ += drivers/ao.o
endif

ifeq ($(HAVE_ROARVS), 1)
   OPT_SERV_LIBS += -lroar
   OPT_SERV_OBJ += drivers/roarvs.o
endif

ifeq ($(HAVE_PULSE), 1)
   OPT_SERV_LIBS += -lpulse-simple
   OPT_SERV_OBJ += drivers/pulse.o
endif

ifeq ($(HAVE_MUROAR), 1)
   OPT_SERV_LIBS += -lmuroar
   OPT_SERV_OBJ += drivers/muroar.o
endif

ifeq ($(HAVE_AL), 1)
ifneq ($(findstring Darwin,$(shell uname -a)),)
   OPT_SERV_LIBS += -framework OpenAL
else
   OPT_SERV_LIBS += -lopenal
endif
   OPT_SERV_OBJ += drivers/al.o
endif

ifeq ($(HAVE_PORTAUDIO), 1)
   OPT_SERV_LIBS += -lportaudio
   OPT_SERV_OBJ += drivers/porta.o
endif

ifeq ($(HAVE_JACK), 1)
   OPT_SERV_LIBS += -ljack
   OPT_SERV_OBJ += drivers/jack.o
endif

ifeq ($(HAVE_COREAUDIO), 1)
ifneq ($(findstring Darwin,$(shell uname -a)),)
   OPT_SERV_LIBS += -framework CoreServices -framework CoreAudio -framework AudioUnit
   OPT_SERV_OBJ += drivers/coreaudio.o
endif
endif

ifeq ($(NEED_RT), 1)
   TARGET_SERVER_LIBS += -lrt
   TARGET_LIB_LIBS += -lrt
endif

ifeq ($(HAVE_SAMPLERATE), 1)
   TARGET_SERVER_LIBS += -lsamplerate
else
   TARGET_SERVER_OBJ += resampler.o
endif

TARGET_CLIENT_OBJ = client.o endian.o $(TARGET_LIB_OBJ_STATIC)

TARGET_SERVER_LIBS += $(OPT_SERV_LIBS)
TARGET_SERVER_OBJ += $(OPT_SERV_OBJ) audio.o endian.o daemon.o rsound-common.o proto.o

all: lib client server

client: check-outdated-config $(TARGET_CLIENT)

lib:check-outdated-config $(TARGET_LIB) 

server: check-outdated-config $(TARGET_SERVER)

check-outdated-config:
	@[ -f config.h ] || (echo "Cannot locate config.h, aborting ..." && /bin/false)
	@[ config.h -nt ../configure ] || (echo "Configure script has been updated. Please run configure again." && /bin/false)
	@[ config.mk -nt ../configure ] || (echo "Configure script has been updated. Please run configure again." && /bin/false)


$(TARGET_CLIENT): $(TARGET_CLIENT_OBJ)
	@echo "LD $@"
	@$(CC) $(CFLAGS) -o $(TARGET_CLIENT) $(TARGET_CLIENT_OBJ) $(TARGET_LIB_LIBS)


$(TARGET_LIB): $(TARGET_LIB_OBJ) $(TARGET_LIB_OBJ_STATIC)
	@echo "LD $@"
ifeq ($(PLATFORM),OSX)
	@$(CC) -dynamiclib -o $(TARGET_LIB) $(CFLAGS) $(TARGET_LIB_OBJ) $(TARGET_LIB_LIBS) -install_name $(PREFIX)/lib/librsound.dylib
else
	@$(CC) -shared -Wl,-soname,librsound.so.3 -o $(TARGET_LIB) $(CFLAGS) $(TARGET_LIB_OBJ) $(TARGET_LIB_LIBS) -fPIC
	@ln -sf librsound.so.3.0.0 librsound/librsound.so.3
	@ln -sf librsound.so.3.0.0 librsound/librsound.so
endif

$(TARGET_SERVER): $(TARGET_SERVER_OBJ)
	@echo "LD $@"
	@$(CC) -o $(TARGET_SERVER) $(TARGET_SERVER_OBJ) $(TARGET_SERVER_LIBS) $(CFLAGS)


librsound/librsound.o: librsound/librsound.c config.h
	@echo "CC $<"
	@$(CC) $(CFLAGS) -fPIC -c -o $@ $<

librsound/buffer.o: librsound/buffer.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -fPIC -c -o $@ $<

$(TARGET_LIB_OBJ_STATIC): $(TARGET_LIB_OBJ)
	@echo "AR $@"
	@$(AR) rcs $(TARGET_LIB_OBJ_STATIC) $(TARGET_LIB_OBJ) >/dev/null 2>/dev/null

	
%.o: %.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

rsound-common.o: config.h

audio.o: config.h

clean:
	rm -rf rsd 
	rm -rf rsdplay 
	rm -rf librsound/librsound.so*
	rm -rf $(TARGET_LIB_OBJ)
	rm -rf librsound/librsound.a
	rm -rf *.o
	rm -rf drivers/*.o
	rm -rf rsound.pc

distclean: clean
	rm -rf config.h
	rm -rf config.mk

install: install-lib install-server install-client


install-lib: $(TARGET_LIB) $(TARGET_LIB_OBJ_STATIC) install-pkgconf
	mkdir -p $(DESTDIR)$(PREFIX)/lib/pkgconfig 2>/dev/null || /bin/true
	mkdir -p $(DESTDIR)$(PREFIX)/include 2>/dev/null || /bin/true
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1 2>/dev/null || /bin/true
	mkdir -p $(DESTDIR)$(PREFIX)/lib/ckport/db 2>/dev/null || /bin/true
	install -m755 $(TARGET_LIB) $(DESTDIR)$(PREFIX)/lib
	install -m644 ../ckport/librsound.ckport $(DESTDIR)$(PREFIX)/lib/ckport/db
ifneq ($(PLATFORM),OSX)
	cp -P librsound/librsound.so librsound/librsound.so.3 $(DESTDIR)$(PREFIX)/lib
endif
	install -m644 $(TARGET_LIB_OBJ_STATIC) $(DESTDIR)$(PREFIX)/lib
	install -m644 librsound/rsound.h $(DESTDIR)$(PREFIX)/include
	install -m644 rsound.pc $(DESTDIR)$(PREFIX)/lib/pkgconfig
	install -m644 ../doc/rsd.1 $(DESTDIR)$(PREFIX)/share/man/man1
	install -m644 ../doc/rsdplay.1 $(DESTDIR)$(PREFIX)/share/man/man1

install-server: $(TARGET_SERVER)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m755 $(TARGET_SERVER) $(DESTDIR)$(PREFIX)/bin

install-client: $(TARGET_CLIENT)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m755 $(TARGET_CLIENT) $(DESTDIR)$(PREFIX)/bin

install-pkgconf:
	@echo "prefix=$(PREFIX)" > rsound.pc
	@echo "exec_prefix=\$${prefix}" >> rsound.pc
	@echo "libdir=\$${prefix}/lib" >> rsound.pc
	@echo "includedir=\$${prefix}/include" >> rsound.pc
	@echo "" >> rsound.pc
	@echo "Name: RSound" >> rsound.pc
	@echo "Description: An audio library designed for seamless networked transfer of audio streams." >> rsound.pc
	@echo "Requires:" >> rsound.pc
	@echo "Version: $(RSD_VERSION)" >> rsound.pc
	@echo "Libs: -L\$${prefix}/lib -lrsound" >> rsound.pc
	@echo "Cflags: -I\$${prefix}/include" >> rsound.pc

uninstall:
	rm -rf $(PREFIX)/bin/rsd
	rm -rf $(PREFIX)/bin/rsdplay
	rm -rf $(PREFIX)/lib/librsound.*
	rm -rf $(PREFIX)/include/rsound.h
	rm -rf $(PREFIX)/lib/pkgconfig/rsound.pc
	rm -rf $(PREFIX)/share/man/man1/rsd.1
	rm -rf $(PREFIX)/share/man/man1/rsdplay.1


.PHONY: clean distclean client lib server install install-lib install-server install-client all uninstall check-outdated-config
