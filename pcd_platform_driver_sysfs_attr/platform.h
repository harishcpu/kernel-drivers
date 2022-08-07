/* -----------------------------------------------------------------------------
 * 	Filename   : platform.h	
 * 	Description: This file contains platform data of the pcdevice 
 *	Created    : Thursday 7 July 2022 02:08:10 IST
 *	Version    : 1.0
 *	Author     : Harish Kumar, Linux Devleloper, harishec031@gmail.com
 *	
 *	Copyright (c) 2022 Harish Kumar
 * ----------------------------------------------------------------------------- */

/* platform data */
struct pcdev_platform_data {
    int size;
    int perm;
    const char *serial_number;
};

#define RDWR    0x11
#define RDONLY  0x01
#define WRONLY  0x10
