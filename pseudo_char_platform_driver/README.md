# Pseudo-character Platform Device Driver

Logs:
```
# insmod pcd_device_setup.ko 
# insmod pcd_platform_driver.ko  

# lsmod | head
Module                  Size  Used by
pcd_platform_driver    16384  0
pcd_device_setup       20480  0
isofs                  49152  1
vboxvideo              24576  0
drm_vram_helper        24576  1 vboxvideo
nls_iso8859_1          16384  1
intel_rapl_msr         20480  0
snd_intel8x0           45056  2
snd_ac97_codec        139264  1 snd_intel8x0

# dmesg 
[ 1181.251186] pcdev_platform_init : Device setup module loaded
[ 1187.695327] pcd_platform_driver_probe :A device is detected
[ 1187.695332] pcd_platform_driver_probe :Device serial number = PCDEVABC111
[ 1187.695333] pcd_platform_driver_probe :Device size = 512
[ 1187.695334] pcd_platform_driver_probe :Device permission = 17
[ 1187.695335] pcd_platform_driver_probe :Config item 1 = 60
[ 1187.695336] pcd_platform_driver_probe :Config item 2 = 21
[ 1187.695391] pcd_platform_driver_probe :Probe was successful
[ 1187.695402] pcd_platform_driver_probe :A device is detected
[ 1187.695403] pcd_platform_driver_probe :Device serial number = PCDEVXYZ222
[ 1187.695404] pcd_platform_driver_probe :Device size = 1024
[ 1187.695404] pcd_platform_driver_probe :Device permission = 17
[ 1187.695405] pcd_platform_driver_probe :Config item 1 = 50
[ 1187.695405] pcd_platform_driver_probe :Config item 2 = 22
[ 1187.695421] pcd_platform_driver_probe :Probe was successful
[ 1187.695426] pcd_platform_driver_probe :A device is detected
[ 1187.695427] pcd_platform_driver_probe :Device serial number = PCDEVABC333
[ 1187.695428] pcd_platform_driver_probe :Device size = 128
[ 1187.695428] pcd_platform_driver_probe :Device permission = 1
[ 1187.695429] pcd_platform_driver_probe :Config item 1 = 40
[ 1187.695429] pcd_platform_driver_probe :Config item 2 = 23
[ 1187.695443] pcd_platform_driver_probe :Probe was successful
[ 1187.695449] pcd_platform_driver_probe :A device is detected
[ 1187.695450] pcd_platform_driver_probe :Device serial number = PCDEVXYZ444
[ 1187.695450] pcd_platform_driver_probe :Device size = 32
[ 1187.695451] pcd_platform_driver_probe :Device permission = 16
[ 1187.695451] pcd_platform_driver_probe :Config item 1 = 30
[ 1187.695452] pcd_platform_driver_probe :Config item 2 = 24
[ 1187.695464] pcd_platform_driver_probe :Probe was successful
[ 1187.695471] pcd_platform_driver_init :pcd platform driver loaded

# ls -l /dev/pcdev-
pcdev-0  pcdev-1  pcdev-2  pcdev-3  

# ls -l /sys/class/pcd_class/pcdev-
pcdev-0/ pcdev-1/ pcdev-2/ pcdev-3/ 

# ls -l /sys/class/pcd_class/pcdev-0/
dev        power/     subsystem/ uevent     

# cat /sys/class/pcd_class/pcdev-0/dev 
239:0

# cat /sys/class/pcd_class/pcdev-0/uevent 
MAJOR=239
MINOR=0
DEVNAME=pcdev-0

# cp ../003pseudo_character_driver_multiple_device_nodes/file.txt .

# dd if=file.txt of=/dev/pcdev-0 
dd: writing to '/dev/pcdev-0': Cannot allocate memory
2+0 records in
1+0 records out
512 bytes copied, 0.000108487 s, 4.7 MB/s
root@harish-vm:/home/harish/ldd/custom_drivers/pseudo_char_platform_driver# dmesg | tail
[ 1336.446485] pcd_open :minor access = 0
[ 1336.446490] pcd_open :open is successful
[ 1336.446508] pcd_write :write requested for 512 bytes 
[ 1336.446509] pcd_write :current file position = 0
[ 1336.446510] pcd_write :number of bytes successfully written = 512
[ 1336.446511] pcd_write :updated file position = 512
[ 1336.446512] pcd_write :write requested for 512 bytes 
[ 1336.446512] pcd_write :current file position = 512
[ 1336.446513] pcd_write :no space left on the device
[ 1336.446595] pcd_release :release is successful

# cat /dev/pcdev-0 
The Linux Kernel documentation:
This is the top level of the kernel’s documentation tree. Kernel documentation, like the kernel itself, is very much a work in progress; that is especially true as we work to integrate our many scattered documents into a coherent whole. Please note that improvements to the documentation are welcome; join the linux-doc list at vger.kernel.org if you want to help out.

Licensing documentation:
The following describes the license of the Linux kernel source code (GPLv2), how to# 

# dd if=file.txt of=/dev/pcdev-1 count=1
1+0 records in
1+0 records out
512 bytes copied, 6.8319e-05 s, 7.5 MB/s

# dmesg | tail
[ 1371.587193] pcd_read :number of bytes successfully read = 0
[ 1371.587194] pcd_read :updated file position = 512
[ 1371.587198] pcd_release :release is successful
[ 1400.834180] pcd_open :minor access = 1
[ 1400.834184] pcd_open :open is successful
[ 1400.834191] pcd_write :write requested for 512 bytes 
[ 1400.834192] pcd_write :current file position = 0
[ 1400.834193] pcd_write :number of bytes successfully written = 512
[ 1400.834194] pcd_write :updated file position = 512
[ 1400.834196] pcd_release :release is successful
root@harish-vm:/home/harish/ldd/custom_drivers/pseudo_char_platform_driver# dmesg | tail -n 20
[ 1336.446513] pcd_write :no space left on the device
[ 1336.446595] pcd_release :release is successful
[ 1371.587165] pcd_open :minor access = 0
[ 1371.587170] pcd_open :open is successful
[ 1371.587175] pcd_read :read requested for 131072 bytes 
[ 1371.587176] pcd_read :current file position = 0
[ 1371.587178] pcd_read :number of bytes successfully read = 512
[ 1371.587179] pcd_read :updated file position = 512
[ 1371.587192] pcd_read :read requested for 131072 bytes 
[ 1371.587193] pcd_read :current file position = 512
[ 1371.587193] pcd_read :number of bytes successfully read = 0
[ 1371.587194] pcd_read :updated file position = 512
[ 1371.587198] pcd_release :release is successful
[ 1400.834180] pcd_open :minor access = 1
[ 1400.834184] pcd_open :open is successful
[ 1400.834191] pcd_write :write requested for 512 bytes 
[ 1400.834192] pcd_write :current file position = 0
[ 1400.834193] pcd_write :number of bytes successfully written = 512
[ 1400.834194] pcd_write :updated file position = 512
[ 1400.834196] pcd_release :release is successful

# cat /dev/pcdev-1
The Linux Kernel documentation:
This is the top level of the kernel’s documentation tree. Kernel documentation, like the kernel itself, is very much a work in progress; that is especially true as we work to integrate our many scattered documents into a coherent whole. Please note that improvements to the documentation are welcome; join the linux-doc list at vger.kernel.org if you want to help out.

Licensing documentation:
The following describes the license of the Linux kernel source code (GPLv2), how toroot@harish-vm:/home/harish/ldd/custom_drivers/pseudo_char_platform_driver# 
root@harish-vm:/home/harish/ldd/custom_drivers/pseudo_char_platform_driver# dmesg | tail -n 20
[ 1371.587194] pcd_read :updated file position = 512
[ 1371.587198] pcd_release :release is successful
[ 1400.834180] pcd_open :minor access = 1
[ 1400.834184] pcd_open :open is successful
[ 1400.834191] pcd_write :write requested for 512 bytes 
[ 1400.834192] pcd_write :current file position = 0
[ 1400.834193] pcd_write :number of bytes successfully written = 512
[ 1400.834194] pcd_write :updated file position = 512
[ 1400.834196] pcd_release :release is successful
[ 1432.192847] pcd_open :minor access = 1
[ 1432.192851] pcd_open :open is successful
[ 1432.192857] pcd_read :read requested for 131072 bytes 
[ 1432.192858] pcd_read :current file position = 0
[ 1432.192860] pcd_read :number of bytes successfully read = 1024
[ 1432.192861] pcd_read :updated file position = 1024
[ 1432.192875] pcd_read :read requested for 131072 bytes 
[ 1432.192876] pcd_read :current file position = 1024
[ 1432.192876] pcd_read :number of bytes successfully read = 0
[ 1432.192876] pcd_read :updated file position = 1024
[ 1432.192881] pcd_release :release is successful
```
