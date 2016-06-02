/**
 * @file usr-lcd-control.c
 * @author Daniel Obermaier
 * @date 02.06.2016
 * @version 0.1
 * @brief a linux user space program that communicates with the hd44780 linux kernel module (LKM).
 * to work with the device the /dev/hd44780 must be called
*/

#include "stdio.h"
#include "fcntl.h"
#include "errno.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "ioctl_header.h"


int main(int argc, char **argv){

char buff[128];

char s_write[100] = "hello linuxloverboys!";
int dev = open("/dev/hd44780", O_WRONLY);	//open device file -> WRITE ONLY


if(dev == -1){
	printf("can't open device file\n");
	return -1;
}

//try to write to device
int ret = write(dev, s_write, 25);

if(ret < 0){
	perror("cant write to devicefile\n");
	return errno;
}


close(dev);		//close device afterwards

return 0;
}


