
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


SUBDIRS = $(shell ls -d */)

SRC=$(wildcard *.c)
OBJ=$(patsubst %.c,%.o,$(SRC) )
INCLUDEPATH := -I../server -I../server/include -I ../common


all : app 

app : clean $(OBJ)
	@for i in $(SUBDIRS); do \
	echo -e "\033[44;37;1m +++ Compiling $$i...\033[0m"; \
	make -C $$i || exit $$?; \
	done
	
%.o : %.c
	$(GCC) $(CFLAGS_NORMAL) $(INCLUDEPATH) $(MACRO) -o $@ $< ../common/proc_sock_path.o 
		
install:
	@for i in $(SUBDIRS); do \
	echo -e "\033[44;37;1m +++ Installing $$i...\033[0m"; \
	make -C $$i install || exit $$?; \
	done
		

clean :
	@for i in $(SUBDIRS); do \
	echo -e "\033[44;37;1m +++ Cleaning $$i...\033[0m"; \
	make -C $$i clean;\
	done
	-$(RM) $(OBJ)
	
	
.PHONY : all app clean 
	
