#pragma once

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt, __func__

struct device_config {
    int config_item1;
    int config_item2;
};

enum pcdev_names {
    PCDEVA1X,
    PCDEVB1X,
    PCDEVC1X,
    PCDEVD1X
};

/* Private data of a single device */
struct pcdev_private_data {
    struct pcdev_platform_data pdata;
    char *buffer;
    dev_t device_number;
    struct cdev pcd_cdev;
};

/* Private data of the driver */
struct pcdrv_private_data {
    int total_devices;
    dev_t device_num_base;
    struct class *class_pcd;
    struct device *device_pcd;
};

int pcd_open(struct inode *inode, struct file *fptr); 

u32 check_permission(u32 dev_perm, fmode_t access_mode); 

int pcd_release (struct inode *inode, struct file *fptr);

ssize_t pcd_read(struct file *fptr, char __user *buff, size_t count, loff_t *f_pos);

ssize_t pcd_write(struct file *fptr, const char __user *buff, size_t count, loff_t *f_pos);

loff_t pcd_lseek(struct file *fptr, loff_t offset, int whence); 

