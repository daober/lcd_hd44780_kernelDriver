#very basic makefile for lcd-kernel module

obj-m += hd44780.o

default:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	rm -rf *.o *~core .depend .*.cmd *.ko *.mod.c .tmp_versions

