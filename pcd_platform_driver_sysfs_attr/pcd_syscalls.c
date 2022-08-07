#include "pcd_platform_driver_sysfs.h"

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
    ret = check_permission(pcdev_data->pdata.perm, fptr->f_mode);
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
    u32 max_size = pcdev_data->pdata.size;

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
    u32 max_size = pcdev_data->pdata.size;

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
    u32 max_size = pcdev_data->pdata.size;
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
