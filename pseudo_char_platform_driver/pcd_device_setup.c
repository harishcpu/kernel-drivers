/* -----------------------------------------------------------------------------
 * 	Filename   : pcd_device_setup.c	
 * 	Description: This is the module which registers platform devices with kernel 
 *	Created    : Thursday 7 July 2022 02:08:10 IST
 *	Version    : 1.0
 *	Author     : Harish Kumar, Linux Devleloper, harishec031@gmail.com
 *	
 *	Copyright (c) 2022 Harish Kumar
 * ----------------------------------------------------------------------------- */


#include <linux/module.h>
#include <linux/platform_device.h>
#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

void pcdev_release(struct device *dev) {
    pr_info("Device released\n");
}

/* Create two platform data */
struct pcdev_platform_data pcdev_pltf_data[4] = {
    [0] = {
        .size = 512,
        .perm = RDWR,
        .serial_number = "PCDEVABC111"
    },
    [1] = {
        .size = 1024,
        .perm = RDWR,
        .serial_number = "PCDEVXYZ222"
    },
    [2] = {
        .size = 128,
        .perm = RDONLY,
        .serial_number = "PCDEVABC333"
    },
    [3] = {
        .size = 32,
        .perm = WRONLY,
        .serial_number = "PCDEVXYZ444"
    }


};

/* Create two platform devices */
struct platform_device platform_pcdev_1 = {
    .name = "pcdev-A1x",
    .id   = 0,
    .dev  = {
        .platform_data = &pcdev_pltf_data[0],
        .release       = pcdev_release
    }
};

struct platform_device platform_pcdev_2 = {
    .name = "pcdev-B1x",
    .id   = 1,
    .dev = {
        .platform_data = &pcdev_pltf_data[1],
        .release       = pcdev_release
    }
};

struct platform_device platform_pcdev_3 = {
    .name = "pcdev-C1x",
    .id   = 2,
    .dev = {
        .platform_data = &pcdev_pltf_data[2],
        .release       = pcdev_release
    }
};

struct platform_device platform_pcdev_4 = {
    .name = "pcdev-D1x",
    .id   = 3,
    .dev = {
        .platform_data = &pcdev_pltf_data[3],
        .release       = pcdev_release
    }
};

struct platform_device *platform_pcdevs[] = {
    &platform_pcdev_1,
    &platform_pcdev_2,
    &platform_pcdev_3,
    &platform_pcdev_4
};

static int __init pcdev_platform_init(void) {
    /* register platform device */
    //platform_device_register(&platform_pcdev_1);
    //platform_device_register(&platform_pcdev_2);
    platform_add_devices(platform_pcdevs, ARRAY_SIZE(platform_pcdevs));

    pr_info("Device setup module loaded\n");
    return 0;
}

static void __exit pcdev_platform_exit(void) {
    platform_device_unregister(&platform_pcdev_1);
    platform_device_unregister(&platform_pcdev_2);
    platform_device_unregister(&platform_pcdev_3);
    platform_device_unregister(&platform_pcdev_4);

    pr_info("Device setup module unloaded\n");
    return;
}

module_init(pcdev_platform_init);
module_exit(pcdev_platform_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("harish");
MODULE_DESCRIPTION("Module which registers platform devices");
