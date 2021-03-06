
ifeq ($(CC),)
   CC := gcc
endif

ifeq ($(CC),cc)
   CC := gcc
endif

CFLAGS := -O2 -std=gnu99 -g -Wall
INCLUDE_DIRS := -Iinclude -I../librsound -Iinclude/pthread -Imuroar
LIB_DIRS := -Lbin -Llib

TARGET_SERVER = bin/rsd.exe
DEFS = -D_DS -D_MUROAR
TARGET_CLIENT = bin/rsdplay.exe
TARGET_LIB = bin/rsound.dll
TARGET_LIB_OBJ = ../librsound/librsound.o src/poll.o src/pthread.o ../librsound/buffer.o
TARGET_LIB_IMPLIB = lib/librsound.a

TARGET_SERVER_LIBS = -lws2_32
TARGET_LIB_LIBS = -lws2_32
OPT_SERV_LIBS = -ldsound -luuid
OPT_SERV_OBJ = ../drivers/dsound.o ../drivers/muroar.o muroar/muroar.o muroar/muroario.o

TARGET_CLIENT_OBJ = ../client.o ../endian.o
TARGET_CLIENT_LIBS = -lrsound -lws2_32

TARGET_SERVER_LIBS += $(OPT_SERV_LIBS)
TARGET_SERVER_OBJ = $(OPT_SERV_OBJ) ../audio.o ../endian.o ../daemon.o ../rsound-common.o ../proto.o ../resampler.o src/poll.o src/pthread.o

TARGET_DIST = rsound-win32-1.1.zip
DIST_EXTRAS = README.txt include/rsound.h COPYING.txt

all: client lib server

client: $(TARGET_CLIENT)

lib: $(TARGET_LIB)

server: $(TARGET_SERVER)

$(TARGET_CLIENT): $(TARGET_CLIENT_OBJ) $(TARGET_LIB)
	@mkdir -p bin
	@echo "LD $@"
	@$(CC) -s $(CFLAGS) -o $(TARGET_CLIENT) $(TARGET_CLIENT_OBJ) $(LIB_DIRS) $(TARGET_CLIENT_LIBS)


$(TARGET_LIB): $(TARGET_LIB_OBJ)
	@mkdir -p bin
	@mkdir -p lib
	@echo "LD $@"
	@$(CC) -shared -Wl,--out-implib,$(TARGET_LIB_IMPLIB) -o $(TARGET_LIB) $(TARGET_LIB_OBJ) $(TARGET_LIB_LIBS) $(LIB_DIRS)

$(TARGET_SERVER): $(TARGET_SERVER_OBJ)
	@mkdir -p bin
	@echo "LD $@"
	@$(CC) -s -o $(TARGET_SERVER) $(TARGET_SERVER_OBJ) $(TARGET_SERVER_LIBS) $(LIB_DIRS)

%.o: %.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(DEFS) -c -o $@ $<

clean:
	rm -f $(TARGET_SERVER)
	rm -f $(TARGET_CLIENT)
	rm -f $(TARGET_CLIENT_OBJ)
	rm -f $(TARGET_LIB)
	rm -f $(TARGET_LIB_OBJ)
	rm -f $(TARGET_LIB_IMPLIB)
	rm -f $(TARGET_SERVER_OBJ)

distclean: clean
	rm -f config.h
	rm -f config.mk

dist: all
	@cp -r ../librsound/rsound.h include/rsound.h
	zip -r $(TARGET_DIST) $(DIST_EXTRAS) $(TARGET_SERVER) $(TARGET_CLIENT) $(TARGET_LIB) $(TARGET_LIB_IMPLIB)
	@rm -r include/rsound.h

.PHONY: clean distclean client lib server all uninstall dist
