###

ver = REL
GCC = $(CROSS_COMPILE)gcc

CFLAGS_NORMAL = -O -Wall -c

PRJ_NAME = cli

RM := /bin/rm -rf

ifeq ($(ver), DBG)
MACRO := -D__DEBUG_PRINT
CFLAGS_NORMAL += -g
else
MACRO :=
endif

TARGET_DIR := ../../../bin
LIBS := -lpthread 
LIBPATH := -L../
INCLUDEPATH := -I../../server -I../../server/include  -I../  -I../../common 
SRC  := ./
SSRC := $(shell find $(SRC) -name '*.c')
SOBJS := $(SSRC:%.c=%.o)

all: $(PRJ_NAME)

$(PRJ_NAME): $(SOBJS)
	$(GCC) $(LIBPATH) -o $@ $^ $(LIBS)  ../client_common.o ../../common/proc_sock_path.o 

%.o : %.c 
	$(GCC) $(CFLAGS_NORMAL) $(MACRO) $(INCLUDEPATH) -o $@ $<

install:
	install $(PRJ_NAME) $(TARGET_DIR)

clean:
	-$(RM) $(PRJ_NAME) $(SOBJS)

.PHONY : all install clean
