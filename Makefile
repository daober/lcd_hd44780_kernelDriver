#generic makefile

obj-m += hd44780.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC) usr-lcd-control.c -o usr-lcd-control
	$(CC) usr-print-ip.c -o usr-print-ip

default:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

user:
	$(CC) usr-lcd-control.c -o usr-lcd-control

ip:
	$(CC) usr-print-ip.c -o usr-print-ip

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
