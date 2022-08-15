/* -----------------------------------------------------------------------------
 * 	Filename   : pcd_platform_driver.c	
 * 	Description: A pseudo-character platform driver which handles N platform 
 * 	             pcdevices 
 *	Created    : Thursday 7 August 2022 02:08:10 IST
 *	Version    : 1.0
 *	Author     : Harish Kumar, Embedded Linux Developer, harishec031@gmail.com
 *	
 *	Copyright (c) 2022 Harish Kumar
 * ----------------------------------------------------------------------------- */

#include "pcd_platform_driver_sysfs.h"

struct pcdrv_private_data pcdrv_data;

struct device_config pcdev_config[] = {
    [PCDEVA1X] = {.config_item1 = 60, .config_item2 = 21},
    [PCDEVB1X] = {.config_item1 = 50, .config_item2 = 22},
    [PCDEVC1X] = {.config_item1 = 40, .config_item2 = 23},
    [PCDEVD1X] = {.config_item1 = 30, .config_item2 = 24},
};

/* file operations of the driver */
struct file_operations pcd_fops = {
    .open = pcd_open,
    .read = pcd_read,
    .write = pcd_write,
    .llseek = pcd_lseek,
    .release = pcd_release,
    .owner = THIS_MODULE
};

ssize_t 
show_max_size (struct device *dev, 
               struct device_attribute *attr,
               char *buf) {
    /* get access to the device private data */
    struct pcdev_private_data *dev_data = dev_get_drvdata(dev->parent);

    if(!dev_data) {
        pr_info("null reference by dev_data, %p\n", dev_data);
        return 0;
    }
    return sprintf(buf, "%d\n", dev_data->pdata.size);
}

ssize_t 
show_serial_number (struct device *dev, 
        struct device_attribute *attr,
        char *buf) {
    /* get access to the device private data */
    struct pcdev_private_data *dev_data = dev_get_drvdata(dev->parent);

    if(!dev_data) {
        pr_info("null reference by dev_data, %p\n", dev_data);
        return 0;
    }
    return sprintf(buf, "%s\n", dev_data->pdata.serial_number);
}

ssize_t 
store_max_size (struct device *dev, 
                        struct device_attribute *attr,
                        const char *buf, 
                        size_t count) {
    long result;
    int ret;
    struct pcdev_private_data *dev_data = dev_get_drvdata(dev->parent);

    ret = kstrtol(buf, 10, &result);
    if(ret){
        return ret;
    }

    dev_data->pdata.size = result;

    dev_data->buffer = krealloc(dev_data->buffer, dev_data->pdata.size, GFP_KERNEL);

    return count;
}

/* create 2 variables of struct device_attribute */
static DEVICE_ATTR(max_size, S_IRUGO|S_IWUSR, show_max_size, store_max_size);
static DEVICE_ATTR(serial_number, S_IRUGO, show_serial_number, NULL);

struct attribute *pcd_attrs[] = {
    &dev_attr_max_size.attr,
    &dev_attr_serial_number.attr,
    NULL
};

struct attribute_group pcd_attr_group = {
    .attrs = pcd_attrs
};

int pcd_sysfs_create_file(struct device *pcd_dev) {
#if 0
    int ret = sysfs_create_file(&pcd_dev->kobj, &dev_attr_max_size.attr);
    if(ret)
        return ret;
    
   return sysfs_create_file(&pcd_dev->kobj, &dev_attr_serial_number.attr);
#endif
   return sysfs_create_group(&pcd_dev->kobj, &pcd_attr_group);
}

/* Gets called when matched platform device is found */
int pcd_platform_driver_probe (struct platform_device *pdev) {
    u32 ret;
    struct pcdev_private_data *dev_data;
    struct pcdev_platform_data *pdata;
    struct device *dev = &pdev->dev;

    pr_info("A device is detected\n");

    /* 1. Get the platform data  */
    pdata = (struct pcdev_platform_data *)dev_get_platdata(&pdev->dev);
    if(!pdata) {
        pr_info("No platform data available\n");
        ret = -EINVAL;
        return ret;
    }

    /* 2. Dynamically allocate memory for the device private data */
    dev_data = devm_kzalloc(&pdev->dev, sizeof(*dev_data), GFP_KERNEL);
    if(!dev_data) {
        pr_info("Cannot allocate memory\n");
        return -ENOMEM;
    } 

    /* Save the device private data pointer in platform device structure */
    dev_set_drvdata(&pdev->dev, dev_data);

    dev_data->pdata.size = pdata->size;
    dev_data->pdata.perm = pdata->perm;
    dev_data->pdata.serial_number = pdata->serial_number;

    pr_info("Device serial number = %s\n", dev_data->pdata.serial_number);
    pr_info("Device size = %d\n", dev_data->pdata.size);
    pr_info("Device permission = %d\n", dev_data->pdata.perm);
    pr_info("Config item 1 = %d\n", pcdev_config[pdev->id_entry->driver_data].config_item1);
    pr_info("Config item 2 = %d\n", pcdev_config[pdev->id_entry->driver_data].config_item2);

    /* 3. Dynamically allocate memory for the device buffer using size
     *    information from the platform data */
    dev_data->buffer = devm_kzalloc(&pdev->dev, dev_data->pdata.size, GFP_KERNEL);
    if(!dev_data->buffer) {
        pr_info("Cannot allocate memory\n");
        return -ENOMEM;
    }

    /* 4. Get the device number */
    dev_data->device_number = pcdrv_data.device_num_base + pdev->id;

    /* 5. Do cdev init and cdev add */
    cdev_init(&dev_data->pcd_cdev, &pcd_fops);

    dev_data->pcd_cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev_data->pcd_cdev, dev_data->device_number, 1);
    if (ret < 0) {
        pr_err("cdev add failed\n");
        return ret; 
    }
    /* 6. Create device file for the detected platform device */
    pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, dev, dev_data->device_number, NULL, "pcdev-%d", pdev->id);
    if(IS_ERR(pcdrv_data.device_pcd)) {
        pr_err("device create failed\n");
        ret = PTR_ERR(pcdrv_data.device_pcd);
        cdev_del(&dev_data->pcd_cdev);
        return ret;
    }
    
    pcdrv_data.total_devices++;
    ret = pcd_sysfs_create_file(pcdrv_data.device_pcd);
    if(ret) {
        device_destroy(pcdrv_data.class_pcd, dev_data->device_number);
        return ret;
    }

    pr_info("Probe was successful\n");
    return 0;
}

/* Gets called when the device is removed from the system */
int pcd_platform_driver_remove (struct platform_device *pdev) {
    
    struct pcdev_private_data *dev_data = dev_get_drvdata(&pdev->dev);

    /* 1. Remove a device that was created with device_create() */
    device_destroy(pcdrv_data.class_pcd, dev_data->device_number);

    /* 2. Remove a cdev entry from the system */
    cdev_del(&dev_data->pcd_cdev);

    /* 3. Free the memory held by the device */
    /* NOTE: will be taken care by the kernel 
     * since we've used device resource managed kernel api
     */

    pcdrv_data.total_devices--;

    pr_info("A device is removed\n");
    return 0;
}

struct platform_device_id pcdevs_ids[] = {
    [0] = {.name= "pcdev-A1x", .driver_data = PCDEVA1X},
    [1] = {.name= "pcdev-B1x", .driver_data = PCDEVB1X},
    [2] = {.name= "pcdev-C1x", .driver_data = PCDEVC1X},
    [3] = {.name= "pcdev-D1x", .driver_data = PCDEVD1X},
    {} /* NULL terminated */
};

struct platform_driver pcd_platform_driver = {
    .probe = pcd_platform_driver_probe,
    .remove = pcd_platform_driver_remove,
    .id_table = pcdevs_ids,
    .driver = {
        .name = "pseudo-char-device"
    }
};

#define MAX_DEVICES 10
static int __init pcd_platform_driver_init(void)
{
    u32 ret;
    /* 1. Dynamically allocate a device number for MAX_DEVICES */
    ret = alloc_chrdev_region(&pcdrv_data.device_num_base, 0, MAX_DEVICES, "pcdevs");
    if(ret < 0) {
        pr_err("Alloc chrdev failed\n");
        return ret;
    }

    /* 2. Create device class under /sys/class */
    pcdrv_data.class_pcd = class_create(THIS_MODULE, "pcd_class");
    if(IS_ERR(pcdrv_data.class_pcd)) {
        pr_err("class creation failed\n");
        ret = PTR_ERR(pcdrv_data.class_pcd);
        unregister_chrdev_region(pcdrv_data.device_num_base, MAX_DEVICES);
        return ret;
    }

    /* 3. Register a platform driver */
    platform_driver_register(&pcd_platform_driver);
    pr_info("pcd platform driver loaded\n");
    
    return 0;
}

static void __exit pcd_platform_driver_cleanup(void)
{
    /* 1. Unregister the platform driver */
    platform_driver_unregister(&pcd_platform_driver);

    /* 2. Class destroy */
    class_destroy(pcdrv_data.class_pcd);

    /* 3. Unregister device numbers for MAX_DEVICES */
    unregister_chrdev_region(pcdrv_data.device_num_base, MAX_DEVICES);

    pr_info("pcd platform driver unloaded\n");
}


module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("harish");
MODULE_DESCRIPTION("A pseudo-character platform driver which handles N platform pcdevices");
MODULE_INFO(board, "Beaglebone Black REV A5");

