# Pseudo Character Driver
Pseudo-devices are files, usually located in `/dev`, they're like a device file, but instead of acting as a bridge between the operating system and hardware, it's a device driver without an actual device. they usually serve a practical purpose, such as producing random data, or acting as a virtual sinkhole for unwanted data. examples would be files like `/dev/random` or `/dev/null`. <br />

Below are the steps to compile and debug this pseudo character driver:  <br />
```
Compile as follows:
    $ make host
      make -C /lib/modules/5.18.9/build/ M=/home/harish/ldd/custom_drivers/pcd modules 
      make[1]: Entering directory '/home/harish/ldd/source/linux-5.18.9'
        CC [M]  /home/harish/ldd/custom_drivers/pcd/pcd.o
        MODPOST /home/harish/ldd/custom_drivers/pcd/Module.symvers
        CC [M]  /home/harish/ldd/custom_drivers/pcd/pcd.mod.o
        LD [M]  /home/harish/ldd/custom_drivers/pcd/pcd.ko
      make[1]: Leaving directory '/home/harish/ldd/source/linux-5.18.9'

Manual testing:
    $ insmod pcd.ko
    $ dmesg | tail
      [  689.459490] pcd_driver_init :Device number <major>:<minor> = 239:0
      [  689.459706] pcd_driver_init :Module init successful
    $ ls -l /dev/pcd 
      crw------- 1 root root 239, 0 Jul  5 17:16 /dev/pcd
    $ ls -l /sys/class/pcd_class/pcd/
      total 0
      -r--r--r-- 1 root root 4096 Jul  5 17:19 dev
      drwxr-xr-x 2 root root    0 Jul  5 17:19 power
      lrwxrwxrwx 1 root root    0 Jul  5 17:19 subsystem -> ../../../../class/pcd_class
      -rw-r--r-- 1 root root 4096 Jul  5 17:16 uevent
    $ cat /sys/class/pcd_class/pcd/dev 
      239:0
    $ cat /sys/class/pcd_class/pcd/uevent 
      MAJOR=239
      MINOR=0
      DEVNAME=pcd
    $ echo "hello, how do you do?" > /dev/pcd 
    $ dmesg | tail
      [ 1083.662776] pcd_open :open is successful
      [ 1083.662797] pcd_write :write requested for 22 bytes 
      [ 1083.662799] pcd_write :current file position = 0
      [ 1083.662801] pcd_write :number of bytes successfully written = 22
      [ 1083.662802] pcd_write :updated file position = 22
      [ 1083.662805] pcd_release :release is successful
    $ cat /dev/pcd 
      hello, how do you do?
    $ dmesg | tail
      [ 1251.917749] pcd_open :open is successful
      [ 1251.917766] pcd_read :read requested for 131072 bytes 
      [ 1251.917768] pcd_read :current file position = 0
      [ 1251.917772] pcd_read :number of bytes successfully read = 512
      [ 1251.917773] pcd_read :updated file position = 512
      [ 1251.917796] pcd_read :read requested for 131072 bytes 
      [ 1251.917797] pcd_read :current file position = 512
      [ 1251.917798] pcd_read :number of bytes successfully read = 0
      [ 1251.917799] pcd_read :updated file position = 512
      [ 1251.917808] pcd_release :release is successful

    $ ls -l file.txt 
      -rw-r--r-- 1 root root 1866 Jul  5 15:43 file.txt
    $ cp file.txt /dev//pcd 
      cp: error writing '/dev//pcd': Cannot allocate memory
    $ dmesg | tail
      [ 1251.917808] pcd_release :release is successful
      [ 1336.798979] pcd_open :open is successful
      [ 1336.804224] pcd_write :write requested for 1866 bytes 
      [ 1336.804235] pcd_write :current file position = 0
      [ 1336.804238] pcd_write :number of bytes successfully written = 512
      [ 1336.804239] pcd_write :updated file position = 512
      [ 1336.804241] pcd_write :write requested for 1354 bytes 
      [ 1336.804242] pcd_write :current file position = 512
      [ 1336.804243] pcd_write :no space left on the device
      [ 1336.804447] pcd_release :release is successful
    $ rmmod pcd.ko
      [ 1459.847440] pcd_driver_cleanup :Module unload successful
