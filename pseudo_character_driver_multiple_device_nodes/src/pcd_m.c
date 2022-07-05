#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt, __func__

#define NO_OF_DEVICES 4
#define MEM_SIZE_MAX_PCDEV1 1024
#define MEM_SIZE_MAX_PCDEV2 512
#define MEM_SIZE_MAX_PCDEV3 1024
#define MEM_SIZE_MAX_PCDEV4 512

/* pseudo device's memory */
char device_buffer_pcdev1[MEM_SIZE_MAX_PCDEV1];
char device_buffer_pcdev2[MEM_SIZE_MAX_PCDEV2];
char device_buffer_pcdev3[MEM_SIZE_MAX_PCDEV3];
char device_buffer_pcdev4[MEM_SIZE_MAX_PCDEV4];

/* Private data of a single device */
struct pcdev_private_data {
    char *buffer;
    unsigned size;
    const char *serial_number;
    u32 perm;
    /* cdev variable */
    struct cdev pcd_cdev;
};

struct pcdrv_private_data {
    int total_devices;
    /* This holds the device number */
    dev_t device_number;
    struct class *class_pcd;
    struct device *device_pcd;
    struct pcdev_private_data pcdev_data[NO_OF_DEVICES];
};

#define RDONLY 0x01
#define WRONLY 0x10
#define RDWR   0x11

struct pcdrv_private_data pcdrv_data = {
    .total_devices = NO_OF_DEVICES,
    .pcdev_data = {
        [0] = {
            .buffer = device_buffer_pcdev1,
            .size = MEM_SIZE_MAX_PCDEV1,
            .serial_number = "PCDEV1",
            .perm = RDONLY /*RDONLY*/
        },
        [1] = {
            .buffer = device_buffer_pcdev2,
            .size = MEM_SIZE_MAX_PCDEV2,
            .serial_number = "PCDEV2",
            .perm = WRONLY /*WRONLY*/
        },
        [2] = {
            .buffer = device_buffer_pcdev3,
            .size = MEM_SIZE_MAX_PCDEV3,
            .serial_number = "PCDEV3",
            .perm = RDWR /*RDWR*/
        },
        [3] = {
            .buffer = device_buffer_pcdev4,
            .size = MEM_SIZE_MAX_PCDEV4,
            .serial_number = "PCDEV4",
            .perm = 0x11 /*RDWR*/
        }
    }
};

u32 check_permission(u32 dev_perm, fmode_t access_mode) {
   if (dev_perm == RDWR)
      return 0;
   else if ((dev_perm == RDONLY) && ((access_mode & FMODE_READ) && !(access_mode & FMODE_WRITE)))
       return 0;
   else if ((dev_perm == WRONLY) && ((access_mode & FMODE_WRITE) && !(access_mode & FMODE_READ)))
       return 0;

   return -EPERM;
}

/* pseudo char driver open system call */
int pcd_open(struct inode *inode, struct file *fptr) {
    
    u32 ret;
    u32 minor_n;
    struct pcdev_private_data *pcdev_data;

    /* Find which file was attempted to be open by the user space drivers */
    minor_n = MINOR(inode->i_rdev);
    pr_info("minor access = %d\n", minor_n);

    /* Get device's private data */
    pcdev_data = container_of(inode->i_cdev, struct pcdev_private_data, pcd_cdev);
    
    /* To supply device private data to other methods of the driver */
    fptr->private_data = pcdev_data;
    
    /* Check permissions */
    ret = check_permission(pcdev_data->perm, fptr->f_mode);
    (!ret) ? pr_info("open is successful\n")
           : pr_info("open is unsuccessful\n");

    return ret;
}

/* pseudo char driver close system call */
int pcd_release (struct inode *inode, struct file *fptr){
    pr_info("release is successful\n"); 
    return 0;
}

/* pseudo char driver read system call */
ssize_t pcd_read(struct file *fptr, char __user *buff, size_t count, loff_t *f_pos) {

    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)fptr->private_data;
    u32 max_size = pcdev_data->size;

    pr_info("read requested for %zu bytes \n", count);
    pr_info("current file position = %lld\n", *f_pos);
    
    /* Adjust the 'count' */
    if (*f_pos + count  > max_size)
        count = max_size - *f_pos;

    /* copy to user */
    if (copy_to_user(buff, pcdev_data->buffer+(*f_pos), count)) {
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
ssize_t pcd_write(struct file *fptr, const char __user *buff, size_t count, loff_t *f_pos) {
    
    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)fptr->private_data;
    u32 max_size = pcdev_data->size;

    pr_info("write requested for %zu bytes \n", count);
    pr_info("current file position = %lld\n", *f_pos);

    /* Adjust the count */
    if ((*f_pos + count) > max_size) {
        count = max_size - *f_pos;
    }
   
    if(!count) {
    	pr_err("no space left on the device\n");	    
        return -ENOMEM;
    }

    /* copy from user */
    if (copy_from_user(pcdev_data->buffer+(*f_pos), buff, count)) {
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
    
    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)fptr->private_data;
    u32 max_size = pcdev_data->size;
    loff_t temp;

    pr_info("lseek requested\n");
    pr_info("current value of the file position = %lld\n", fptr->f_pos);
   
   /* Take action based on the whence value */
    switch(whence) {
        case SEEK_SET:
            if ((offset > max_size) || (offset < 0))
                return -EINVAL;
            fptr->f_pos = offset;
            break;
        case SEEK_CUR:
            temp = fptr->f_pos + offset;
            if ((temp > max_size) || (temp < 0))
                return -EINVAL;
            fptr->f_pos = temp;
            break;
        case SEEK_END:
            temp = max_size + offset;
            if ((temp > max_size) || (temp < 0))
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


static int __init pcd_driver_init(void)
{
    u32 ret, i;

    /* Dynamically allocate a device a number */
    ret = alloc_chrdev_region(&pcdrv_data.device_number,0,NO_OF_DEVICES,"pcd_devices");
    if(ret < 0) {
        pr_err("alloc chrdev failed\n");
        goto out;
    }

    /* Create device class under /sys/class/ */
    pcdrv_data.class_pcd = class_create(THIS_MODULE, "pcd_class");
    if(IS_ERR(pcdrv_data.class_pcd)) {
        pr_err("class creation failed\n");
        ret = PTR_ERR(pcdrv_data.class_pcd);
        goto unregister_chrdev;
    }

    for(i = 0; i < NO_OF_DEVICES; i++) {
        pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(pcdrv_data.device_number), 
                MINOR(pcdrv_data.device_number + i));

        /* Initialize the cdev structure with file operations */
        cdev_init(&pcdrv_data.pcdev_data[i].pcd_cdev, &pcd_fops);

        /* Register device (cdev structure) with VFS */
        pcdrv_data.pcdev_data[i].pcd_cdev.owner = THIS_MODULE;
        ret = cdev_add(&pcdrv_data.pcdev_data[i].pcd_cdev, pcdrv_data.device_number+i, 1);
        if (ret < 0) {
            pr_err("cdev add failed\n");
            goto cdev_delete;
        }

        /* Populate the sysfs with device information */
        pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, NULL, pcdrv_data.device_number+i, NULL, "pcdev-%d", i+1);
        if(IS_ERR(pcdrv_data.device_pcd)) {
            pr_err("device create failed\n");
            ret = PTR_ERR(pcdrv_data.device_pcd);
            goto class_delete;
        }
    }

    pr_info("Module init successful\n");

    return 0;

class_delete:
cdev_delete:
    for(; i > 0; i--) {
        device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number+i);
        cdev_del(&pcdrv_data.pcdev_data[i].pcd_cdev);
    }
    class_destroy(pcdrv_data.class_pcd);
unregister_chrdev:
    unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);
out:
    pr_info("module insertion failed\n");
    return ret;
}


static void __exit pcd_driver_cleanup(void)
{
    u32 i;
    for(i=0; i < NO_OF_DEVICES; i++) {
        device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number+i);
        cdev_del(&pcdrv_data.pcdev_data[i].pcd_cdev);
    }
    class_destroy(pcdrv_data.class_pcd);
    unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);

    pr_info("Module unload successful\n");

    return;
}


module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("harish");
MODULE_DESCRIPTION("A pseudo-char driver which handles N devices");
MODULE_INFO(board, "Beaglebone Black REV A5");
