#include "stdio.h"
#include "fcntl.h"
#include "errno.h"
#include "sys/ioctl.h"

#include "ioctl_header.h"

int main(){

char array[100] = { 0 };
int cmd = 0;			//command
char arg = 1;			//additional argument
int lcd_dev = open("/dev/hd44780", O_WRONLY);	//open device file

printf("userspace program to control the lcd\n");
printf("0 = clear display\n");
printf("1 = write value to display\n");
printf("2 = show ip on display\n");
printf("3 = exit\n");

while(cmd !=3){
	
scanf("%i", &cmd);

if(lcd_dev == -1){
	printf("can't open device file\n");
	return ENOENT;
}

switch(cmd){
	case 0: 
		//clear_display();
		printf("not yet implemented\n");
		break;
	case 1:
		printf("writing values to display selected\n");
//		write_displayVal(lcd_dev);
		break;
	default:
		printf("not yet implemented\n");
		break;
	}
}

close(lcd_dev);		//close device afterwards
return 0;
}


