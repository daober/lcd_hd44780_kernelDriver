#!/bin/sh

#this simple scripts copies the userspace program
#lcd-control (which also controls the kernel module)
#to the webspace 'scripts' subfolder
#

echo configuring file

chmod 775 usr-lcd-control

cp usr-lcd-control /var/www/html/scripts


echo done
