#include "stdio.h"
#include "fcntl.h"
#include "errno.h"
#include "sys/ioctl.h"

#include "ioctl_header.h"

int main(int argc, char **argv, char **envp){

int devicefile = open("/dev/hd44780", O_WRONLY);

if(devicefile == -1){
	printf("can't open device file\n");
	return -1;
}

if(write(devicefile, array, 50)){
	printf("can't write device file\n");
}
	

close(devicefile);

return 0;
}
