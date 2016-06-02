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
int cmd = 0;			//command
char arg = 1;			//additional argument

char s_write[100] = "im a userspace prog\n";
int lcd_dev = open("/dev/hd44780", O_WRONLY);	//open device file -> WRITE ONLY

//ioctl(lcd_dev, IOCTL_WRITE, buff); 

printf("userspace program to control the lcd\n");
printf("0 = clear display\n");
printf("1 = write value to display\n");
printf("2 = show ip on display\n");
printf("3 = exit\n");

if(lcd_dev == -1){
	printf("can't open device file\n");
	return -1;
}

if(write(lcd_dev, s_write, 25)){
	printf("cant read devicefile\n");
	return -1;
}

//do{
	
//scanf("%i", &cmd);

/*switch(cmd){
	case 0: 
		//clear_display();
		printf("not yet implemented\n");
		break;
	case 1:
		printf("writing values to display selected\n");*/
//		write_displayVal(lcd_dev);
//		break;
//	default:
//		printf("not yet implemented\n");
//		break;
//	}
//}while(cmd != 3);

close(lcd_dev);		//close device afterwards

return 0;
}


