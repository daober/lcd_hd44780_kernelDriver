#generic makefile

obj-m += hd44780.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC) usr-lcd-control.c -o usr-lcd-control

default:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

user:
	$(CC) usr-lcd-control.c -o usr-lcd-control

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
