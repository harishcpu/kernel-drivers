obj-m := pcd_sysfs.o pcd_device_setup.o
pcd_sysfs-objs += pcd_platform_driver_sysfs.o pcd_syscalls.o
HOST_KERN_DIR = /lib/modules/$(shell uname -r)/build/

host:
	make -C $(HOST_KERN_DIR) M=$(PWD) modules 
clean:
	make -C $(HOST_KERN_DIR) M=$(PWD) clean 
help:
	make -C $(HOST_KERN_DIR) M=$(PWD) help 
