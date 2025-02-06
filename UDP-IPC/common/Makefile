
ver = REL
export CROSS_COMPILE = 
GCC = $(CROSS_COMPILE)gcc

CFLAGS_NORMAL = -O -Wall -c

ifeq ($(ver), DBG)
MACRO := -D__DEBUG_PRINT
CFLAGS_NORMAL += -g
else
MACRO :=
endif

SRC=$(wildcard *.c)
OBJ=$(patsubst %.c,%.o,$(SRC) )
INCLUDEPATH :=  -I../server/include 

all : $(OBJ)

%.o : %.c
	$(GCC) $(CFLAGS_NORMAL) $(INCLUDEPATH) $(MACRO) -o $@ $< 
		

clean :
	-$(RM) *.o
	
	
.PHONY : all clean 
	
