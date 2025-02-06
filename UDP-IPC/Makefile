SUBDIRS = $(shell ls -d */)

all : app

app : common 
	@for i in $(SUBDIRS); do \
	echo -e "\033[44;37;1m +++ Compiling $$i...\033[0m"; \
	make -C $$i || exit $$?; \
	done

server: common
	@echo -e "\033[44;37;1m +++ Compiling server...\033[0m"; \
	make -C server

common: 
	@echo -e "\033[44;37;1m +++ Compiling common...\033[0m"; \
	make -C common


install:
	@for i in $(SUBDIRS); do \
	echo -e "\033[44;37;1m +++ Installing $$i...\033[0m"; \
	make -C $$i install;  \
	done
	
		
clean :
	@for i in $(SUBDIRS); do \
	echo -e "\033[44;37;1m +++ Cleaning $$i...\033[0m"; \
	make -C $$i clean;\
	done
#	@ find . -name *.ko | xargs rm -f
	
	
.PHONY : all app server common  install clean 
	
