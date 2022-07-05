#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt, __func__

#define DEV_MEM_SIZE 512

/* pseudo device's memory */
char device_buffer[DEV_MEM_SIZE];

/* This holds the device number */
dev_t device_number;

/* cdev variable */
struct cdev pcd_cdev;

/* pseudo char driver open system call */
int pcd_open(struct inode *inode, struct file *fptr) {
    pr_info("open is successful\n"); 
    return 0;
}

/* pseudo char driver close system call */
int pcd_release (struct inode *inode, struct file *fptr){
    pr_info("release is successful\n"); 
    return 0;
}

/* pseudo char driver read system call */
ssize_t pcd_read(struct file *fptr, char __user *buff, size_t count, loff_t *f_pos) {
    
    pr_info("read requested for %zu bytes \n", count);
    pr_info("current file position = %lld\n", *f_pos);
    
    /* Adjust the count */
    if (*f_pos + count  > DEV_MEM_SIZE)
        count = DEV_MEM_SIZE - *f_pos;

    /* copy to user */
    if (copy_to_user(buff, &device_buffer[*f_pos], count)) {
        return -EFAULT;
    }

    /* Update the current file position */
    *f_pos  += count;

    pr_info("number of bytes successfully read = %zu\n", count);
    pr_info("updated file position = %lld\n", *f_pos);

    /* Return the number of bytes successfully read */
    return count;
}

/* pseudo char driver write system call */
ssize_t pcd_write(struct file *ptr, const char __user *buff, size_t count, loff_t *f_pos) {
    
    pr_info("write requested for %zu bytes \n", count);
    pr_info("current file position = %lld\n", *f_pos);

    /* Adjust the count */
    if ((*f_pos + count) > DEV_MEM_SIZE) {
        count = DEV_MEM_SIZE - *f_pos;
    }
   
    if(!count) {
    	pr_err("no space left on the device\n");	    
        return -ENOMEM;
    }

    /* copy from user */
    if (copy_from_user(&device_buffer[*f_pos], buff, count)) {
        return -EFAULT;
    }

    /* Update the current file position */
    *f_pos += count;

    pr_info("number of bytes successfully written = %zu\n", count);
    pr_info("updated file position = %lld\n", *f_pos);
    
    /* Return the number of bytes written */
    return count;
}

/* pseudo char driver lseek system call */
loff_t pcd_lseek(struct file *fptr, loff_t offset, int whence) {
    
    loff_t temp;

    pr_info("lseek requested\n");
    pr_info("current value of the file position = %lld\n", fptr->f_pos);
   
    /* Check offset */

   /* Take action based on the whence value */
    switch(whence) {
        case SEEK_SET:
            if ((offset > DEV_MEM_SIZE) || (offset < 0))
                return -EINVAL;
            fptr->f_pos = offset;
            break;
        case SEEK_CUR:
            temp = fptr->f_pos + offset;
            if ((temp > DEV_MEM_SIZE) || (temp < 0))
                return -EINVAL;
            fptr->f_pos = temp;
            break;
        case SEEK_END:
            temp = DEV_MEM_SIZE + offset;
            if ((temp > DEV_MEM_SIZE) || (temp < 0))
                return -EINVAL;
            fptr->f_pos = temp; 
            break;
        default:
            return -EINVAL;
    }

    pr_info("updated value of the file position = %lld\n", fptr->f_pos);
    return fptr->f_pos;;
}

/* file operations of the driver */
struct file_operations pcd_fops = {
    .open = pcd_open,
    .read = pcd_read,
    .write = pcd_write,
    .llseek = pcd_lseek,
    .release = pcd_release,
    .owner = THIS_MODULE
};

struct class *class_pcd;
struct device *device_pcd;

static int __init pcd_driver_init(void)
{
    int ret;

    /* Dynamically allocate a device a number */
    ret = alloc_chrdev_region(&device_number,0,1,"pcd_devices");
    if(ret < 0) {
        pr_err("chrdev failed\n");
        goto out;
    }

    pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));

    /* Initialize the cdev structure with file operations */
    cdev_init(&pcd_cdev, &pcd_fops);

    /* Register device (cdev structure) with VFS */
    pcd_cdev.owner = THIS_MODULE;
    ret = cdev_add(&pcd_cdev, device_number, 1);
    if (ret < 0) {
        pr_err("cdev add failed\n");
        goto unregister_chrdev;
    }

    /* Create device class under /sys/class/ */
    class_pcd = class_create(THIS_MODULE, "pcd_class");
    if(IS_ERR(class_pcd)) {
        pr_err("class creation failed\n");
        ret = PTR_ERR(class_pcd);
        goto cdev_delete;
    }
    
    /* Populate the sysfs with device information */
    device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");
    if(IS_ERR(device_pcd)) {
        pr_err("device create failed\n");
        ret = PTR_ERR(device_pcd);
        goto class_delete;
    }

    pr_info("Module init successful\n");

    return 0;
class_delete:
    class_destroy(class_pcd);
cdev_delete:
    cdev_del(&pcd_cdev);
unregister_chrdev:
    unregister_chrdev_region(device_number, 1);
out:
    return ret;
}


static void __exit pcd_driver_cleanup(void)
{
    device_destroy(class_pcd, device_number);
    class_destroy(class_pcd);
    cdev_del(&pcd_cdev);
    unregister_chrdev_region(device_number, 1);

    pr_info("Module unload successful\n");

    return;
}


module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("harish");
MODULE_DESCRIPTION("A pseudo character driver");
MODULE_INFO(board, "Beaglebone Black REV A5");
