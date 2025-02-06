ver = REL
CROSS_COMPILE = 
GCC = $(CROSS_COMPILE)gcc
AR = aarch64-linux-gnu-ar

#GCC = gcc
#AR = ar

mod = AWM

CFLAGS_NORMAL = -O -Wall -c

PRJ_NAME = srv

RM := /bin/rm -rf

ifeq ($(ver), DBG)
MACRO := -D__DEBUG_PRINT
CFLAGS_NORMAL += -g
else ifeq ($(ver), TEST)
MACRO := -D__DEBUG_GGE_TEST
CFLAGS_NORMAL += -g
else
MACRO :=
endif

ifeq ($(mod), metro)
MACRO += -D__GGE_METRO__
endif

TARGET_DIR := ../../bin
LIBS := -lpthread
LIBPATH :=
INCLUDEPATH := -I./include -I./utils -I./queue -I./parser -I../common 
SRC  := ./
SSRC := $(shell find $(SRC) -name '*.c')
SOBJS := $(SSRC:%.c=%.o)

all: clean $(PRJ_NAME)

$(PRJ_NAME): $(SOBJS)
	$(GCC) $(LIBPATH) -o $@ $^ $(LIBS)  ../common/proc_sock_path.o 

%.o : %.c
	$(GCC) $(CFLAGS_NORMAL) $(MACRO) $(INCLUDEPATH) -o $@ $<


install:
	install -d $(TARGET_DIR)
	install $(PRJ_NAME) $(TARGET_DIR)

clean:
	-$(RM) $(PRJ_NAME) $(SOBJS)

.PHONY : all install clean
