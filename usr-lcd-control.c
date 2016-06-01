#include "stdio.h"
#include "fcntl.h"
#include "errno.h"
#include <unistd.h>
#include <stdlib.h>
//#include "sys/ioctl.h"

#include "ioctl_header.h"

int main(int argc, char **argv){

char buff[128];
int cmd = 0;			//command
char arg = 1;			//additional argument

char s_write[100] = "im a userspace prog\n";
int lcd_dev = open("/dev/hd44780", O_WRONLY);	//open device file

//ioctl(lcd_dev, IOCTL_WRITE, buff); 

printf("userspace program to control the lcd\n");
printf("0 = clear display\n");
printf("1 = write value to display\n");
printf("2 = show ip on display\n");
printf("3 = exit\n");

fflush(stdout);

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


