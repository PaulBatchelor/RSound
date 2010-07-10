
ifeq ($(CC),)
   CC := gcc
endif

CFLAGS := -O2 -Wall -Wextra -std=gnu99
INCLUDE_DIRS := -Iinclude -Iinclude/pthread
LIB_DIRS := -Lbin -Llib

TARGET_SERVER = rsd.exe
TARGET_CLIENT = rsdplay.exe
TARGET_LIB = bin/rsound.dll
TARGET_LIB_OBJ = ../librsound/librsound.o src/poll.o
TARGET_LIB_IMPLIB = lib/librsound.a


TARGET_SERVER_LIBS = -lpthreadGC2 -lws2_32
TARGET_LIB_LIBS = -lpthreadGC2 -lws2_32
OPT_SERV_LIBS = lib/OpenAL32.lib
OPT_SERV_OBJ = ../al.o

TARGET_CLIENT_OBJ = ../client.o ../endian.o
TARGET_CLIENT_LIBS = -lrsound -lws2_32

TARGET_SERVER_LIBS += $(OPT_SERV_LIBS)
TARGET_SERVER_OBJ = $(OPT_SERV_OBJ) ../audio.o ../endian.o ../daemon.o ../rsound-common.o ../proto.o src/poll.o

all: client lib server

client: $(TARGET_CLIENT)

lib: $(TARGET_LIB)

server: $(TARGET_SERVER)

$(TARGET_CLIENT): $(TARGET_CLIENT_OBJ) $(TARGET_LIB)
	@echo "LD $@"
	@$(CC) -s $(CFLAGS) -o $(TARGET_CLIENT) $(TARGET_CLIENT_OBJ) $(LIB_DIRS) $(TARGET_CLIENT_LIBS)


$(TARGET_LIB): $(TARGET_LIB_OBJ)
	@echo "LD `echo $@ | sed 's|^\S\S*/\([^.][^.]*\).*|\1.dll|'`"
	@$(CC) -shared -Wl,--out-implib,$(TARGET_LIB_IMPLIB) -o $(TARGET_LIB) $(TARGET_LIB_OBJ) $(TARGET_LIB_LIBS) $(LIB_DIRS)

$(TARGET_SERVER): $(TARGET_SERVER_OBJ)
	@echo "LD $@"
	@$(CC) -s -o $(TARGET_SERVER) $(TARGET_SERVER_OBJ) $(TARGET_SERVER_LIBS) $(LIB_DIRS)

%.o: %.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c -o $@ $<

clean:
	rm -rf $(TARGET_SERVER)
	rm -rf $(TARGET_CLIENT)
	rm -rf $(TARGET_LIB)
	rm -rf $(TARGET_LIB_OBJ)
	rm -rf $(TARGET_LIB_IMPLIB)
	rm -rf ../*.o
	rm -rf ../rsound.pc

distclean: clean
	rm -rf config.h
	rm -rf config.mk

.PHONY: clean distclean client lib server all uninstall