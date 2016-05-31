/*
* IOCTL creates a kernelmessage
* IOCTL_WRITE writes a variable to the character driver and also prints a kernel message
*/

//#define IOCTL		_IO('k', 0, int);
#define IOCTL_WRITE	_IOW('k', 2, int)
#define IOCTL_CLEAR	_IOC('k', 3, int);
