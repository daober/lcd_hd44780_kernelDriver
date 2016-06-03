/**
 * @file usr-lcd-control.c
 * @author Daniel Obermaier
 * @date 02.06.2016
 * @version 0.1
 * @brief a linux user space program that communicates with the hd44780 linux kernel module (LKM).
 * to work with the device the /dev/hd44780 must be called
*/

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ioctl_header.h"


int main(void){

/*static const char* devname = "/dev/hd44780";

int ret = 0;
char buff[128] = "";
int dev = 0;*/
	
//	int dev = open(devname, O_WRONLY);	//open device file -> WRITE ONLY
	
//	ret = write(dev, "                                ", 32);	//clear display
/*	if(dev == -1){
		perror("can't open device file\n");
		return -EIO;
	}

	ret = write(dev, argv[1], 128);		//write string to device

	if(ret < 0){
		perror("cant write to devicefile\n");
		return EIO;
	}*/

//close(dev);		//close device afterwards

return 0;
}


