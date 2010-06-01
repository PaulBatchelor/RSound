include config.mk

TARGET_SERVER = rsd
TARGET_CLIENT = rsdplay
TARGET_LIB = librsound/librsound.so
TARGET_LIB_OBJ = librsound/librsound.o
TARGET_LIB_OBJ_STATIC = librsound/librsound.a

CFLAGS += -Wall

TARGET_SERVER_LIBS = -lpthread
TARGET_LIB_LIBS = -lpthread
OPT_SERV_LIBS =
OPT_SERV_OBJ =

ifeq ($(HAVE_ALSA), 1)
   OPT_SERV_LIBS += -lasound
   OPT_SERV_OBJ += alsa.o
endif

ifeq ($(HAVE_OSS), 1)
ifeq ($(HAVE_OSSAUDIO), 1)
   OPT_SERV_LIBS += -lossaudio
endif
   OPT_SERV_OBJ += oss.o
endif

ifeq ($(HAVE_LIBAO), 1)
   OPT_SERV_LIBS += -lao
   OPT_SERV_OBJ += ao.c
endif

ifeq ($(HAVE_PULSE), 1)
   OPT_SERV_LIBS += -lpulse-simple
   OPT_SERV_OBJ += pulse.o
endif

ifeq ($(HAVE_MUROAR), 1)
   OPT_SERV_LIBS += -lmuroar
   OPT_SERV_OBJ += muroar.o
endif

ifeq ($(HAVE_AL), 1)
   OPT_SERV_LIBS += -lopenal
   OPT_SERV_OBJ += al.o
endif

ifeq ($(HAVE_PORTAUDIO), 1)
   OPT_SERV_LIBS += -lportaudio
   OPT_SERV_OBJ += porta.o
endif

ifeq ($(NEED_RT), 1)
   TARGET_SERVER_LIBS += -lrt
   TARGET_LIB_LIBS += -lrt
endif

TARGET_CLIENT_OBJ = client.o endian.o $(TARGET_LIB_OBJ_STATIC)

TARGET_SERVER_LIBS += $(OPT_SERV_LIBS)
TARGET_SERVER_OBJ = $(OPT_SERV_OBJ) audio.o endian.o daemon.o rsound-common.o proto.o

all: client lib server

client: $(TARGET_CLIENT)

lib: $(TARGET_LIB)

server: $(TARGET_SERVER)

$(TARGET_CLIENT): $(TARGET_CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT) $(TARGET_CLIENT_OBJ) $(TARGET_LIB_LIBS)


$(TARGET_LIB): $(TARGET_LIB_OBJ) $(TARGET_LIB_OBJ_STATIC)
	$(CC) -shared -Wl,-soname,librsound.so.1 -o $(TARGET_LIB) $(TARGET_LIB_OBJ) $(TARGET_LIB_LIBS)

$(TARGET_SERVER): $(TARGET_SERVER_OBJ)
	$(CC) -o $(TARGET_SERVER) $(TARGET_SERVER_OBJ) $(TARGET_SERVER_LIBS)


librsound/librsound.o: librsound/librsound.c config.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

$(TARGET_LIB_OBJ_STATIC): $(TARGET_LIB_OBJ)
	ar r $(TARGET_LIB_OBJ_STATIC) $(TARGET_LIB_OBJ) >/dev/null 2>/dev/null


rsound-common.o: rsound-common.c config.h
	$(CC) $(CFLAGS) -c -o $@ $<
	
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf rsd 
	rm -rf rsdplay 
	rm -rf librsound/*.{o,so,a}
	rm -rf *.o

install: install-lib install-server install-client


install-lib: $(TARGET_LIB) $(TARGET_LIB_OBJ_STATIC)
	install -m755 $(TARGET_LIB) $(DESTDIR)$(PREFIX)/lib
	ln -sf librsound.so $(DESTDIR)$(PREFIX)/lib/librsound.so.1
	install -m644 $(TARGET_LIB_OBJ_STATIC) $(DESTDIR)$(PREFIX)/lib
	install -m644 librsound/rsound.h $(DESTDIR)$(PREFIX)/include

install-server: $(TARGET_SERVER)
	install -m755 $(TARGET_SERVER) $(DESTDIR)$(PREFIX)/bin

install-client: $(TARGET_CLIENT)
	install -m755 $(TARGET_CLIENT) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -rf $(PREFIX)/bin/rsd
	rm -rf $(PREFIX)/bin/rsdplay
	rm -rf $(PREFIX)/lib/librsound.*
	rm -rf $(PREFIX)/include/rsound.h


.PHONY: clean client lib server install install-lib install-server install-client all uninstall