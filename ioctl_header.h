/**
* IOCTL creates a kernelmessage
* IOCTL_WRITE writes a variable to the character driver and also prints a kernel message
* IOCTL_READ reads a variable from the driver 
*/

#include <linux/ioctl.h>

#define MAGIC_NUM 'k' 	/*unique arbitrary number for driver*/

#define IOCTL		_IO(MAGIC_NUM, 0);		/*int argument*/
#define IOCTL_WRITE	_IOW(MAGIC_NUM, 2, int)		/*returns sizeof(int) bytes to the user */
#define IOCTL_READ	_IOR(MAGIC_NUM, 3, int)		/*...*/
