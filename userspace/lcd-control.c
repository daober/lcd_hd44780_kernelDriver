#include "stdio.h"
#include "fcntl.h"
#include "errno.h"
#include "sys/ioctl.h"


int main(void){

int devicefile = open("~/SysProg/hd44780.ko", ORDWR);

if(devicefile == -1){
	printf("Can't open device file\n");
	return -1;
}
	

close(devicefile);

return 0;
}
