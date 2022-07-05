# Pseudo-character driver with multiple device nodes support

Logs:
```
# make host
make -C /lib/modules/5.18.9/build/ M=/home/harish/ldd/custom_drivers/pseudo_char_driver_multiple modules 
make[1]: Entering directory '/home/harish/ldd/source/linux-5.18.9'
  CC [M]  /home/harish/ldd/custom_drivers/pseudo_char_driver_multiple/pcd_m.o
  MODPOST /home/harish/ldd/custom_drivers/pseudo_char_driver_multiple/Module.symvers
  CC [M]  /home/harish/ldd/custom_drivers/pseudo_char_driver_multiple/pcd_m.mod.o
  LD [M]  /home/harish/ldd/custom_drivers/pseudo_char_driver_multiple/pcd_m.ko
make[1]: Leaving directory '/home/harish/ldd/source/linux-5.18.9'

# insmod pcd_m.ko

# dmesg | tail -n5
[ 3398.548098] pcd_driver_init :Device number <major>:<minor> = 239:0
[ 3398.548321] pcd_driver_init :Device number <major>:<minor> = 239:1
[ 3398.548630] pcd_driver_init :Device number <major>:<minor> = 239:2
[ 3398.549375] pcd_driver_init :Device number <major>:<minor> = 239:3
[ 3398.549426] pcd_driver_init :Module init successful

# ls -l /dev/pcdev-
pcdev-1  pcdev-2  pcdev-3  pcdev-4  

# echo "hello, world!" > /dev/pcdev-1 
bash: /dev/pcdev-1: Operation not permitted

# dmesg | tail -n5
[ 3398.548630] pcd_driver_init :Device number <major>:<minor> = 239:2
[ 3398.549375] pcd_driver_init :Device number <major>:<minor> = 239:3
[ 3398.549426] pcd_driver_init :Module init successful
[ 3465.213421] pcd_open :minor access = 0
[ 3465.213431] pcd_open :open is unsuccessful

# dd if=file.txt of=/dev/pcdev-2 
dd: writing to '/dev/pcdev-2': Cannot allocate memory
2+0 records in
1+0 records out
512 bytes copied, 0.000200577 s, 2.6 MB/s

# dmesg | tail
[ 3603.789686] pcd_open :minor access = 1
[ 3603.789696] pcd_open :open is successful
[ 3603.789711] pcd_write :write requested for 512 bytes 
[ 3603.789713] pcd_write :current file position = 0
[ 3603.789715] pcd_write :number of bytes successfully written = 512
[ 3603.789716] pcd_write :updated file position = 512
[ 3603.789718] pcd_write :write requested for 512 bytes 
[ 3603.789719] pcd_write :current file position = 512
[ 3603.789720] pcd_write :no space left on the device
[ 3603.789891] pcd_release :release is successful

# dd if=file.txt of=/dev/pcdev-2 count=1
1+0 records in
1+0 records out
512 bytes copied, 0.000137689 s, 3.7 MB/s

# dmesg | tail
[ 3603.789719] pcd_write :current file position = 512
[ 3603.789720] pcd_write :no space left on the device
[ 3603.789891] pcd_release :release is successful
[ 3723.084612] pcd_open :minor access = 1
[ 3723.084621] pcd_open :open is successful
[ 3723.084638] pcd_write :write requested for 512 bytes 
[ 3723.084640] pcd_write :current file position = 0
[ 3723.084641] pcd_write :number of bytes successfully written = 512
[ 3723.084642] pcd_write :updated file position = 512
[ 3723.084646] pcd_release :release is successful

# dd if=/dev/pcdev-2 of=dev2.txt count=1
dd: failed to open '/dev/pcdev-2': Operation not permitted

# dmesg | tail
[ 3603.789891] pcd_release :release is successful
[ 3723.084612] pcd_open :minor access = 1
[ 3723.084621] pcd_open :open is successful
[ 3723.084638] pcd_write :write requested for 512 bytes 
[ 3723.084640] pcd_write :current file position = 0
[ 3723.084641] pcd_write :number of bytes successfully written = 512
[ 3723.084642] pcd_write :updated file position = 512
[ 3723.084646] pcd_release :release is successful
[ 3780.819287] pcd_open :minor access = 1
[ 3780.819296] pcd_open :open is unsuccessful
# dd if=file.txt of=/dev/pcdev-3 count=1 bs=100 
1+0 records in
1+0 records out
100 bytes copied, 0.000164781 s, 607 kB/s

# dmesg | tail
[ 3723.084646] pcd_release :release is successful
[ 3780.819287] pcd_open :minor access = 1
[ 3780.819296] pcd_open :open is unsuccessful
[ 3848.278212] pcd_open :minor access = 2
[ 3848.278221] pcd_open :open is successful
[ 3848.278231] pcd_write :write requested for 100 bytes 
[ 3848.278233] pcd_write :current file position = 0
[ 3848.278235] pcd_write :number of bytes successfully written = 100
[ 3848.278236] pcd_write :updated file position = 100
[ 3848.278239] pcd_release :release is successful

# cat /dev/pcdev-3 
The Linux Kernel documentation:
This is the top level of the kernel’s documentation tree. Kernel d

# dmesg | tail
[ 3880.609605] pcd_open :open is successful
[ 3880.609615] pcd_read :read requested for 131072 bytes 
[ 3880.609617] pcd_read :current file position = 0
[ 3880.609621] pcd_read :number of bytes successfully read = 1024
[ 3880.609622] pcd_read :updated file position = 1024
[ 3880.609647] pcd_read :read requested for 131072 bytes 
[ 3880.609648] pcd_read :current file position = 1024
[ 3880.609649] pcd_read :number of bytes successfully read = 0
[ 3880.609650] pcd_read :updated file position = 1024
[ 3880.609659] pcd_release :release is successful
```
