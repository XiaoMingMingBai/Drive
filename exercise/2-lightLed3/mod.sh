rmmod myled.ko
insmod myled.ko
mknod /dev/myled c 242 0
./led1