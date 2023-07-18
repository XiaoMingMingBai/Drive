# Compiling kernel modules
make arch=arm modname=myled
# Compile application
arm-linux-gnueabihf-gcc test.c -o led1
# Copy modules and application
cp myled.ko ~/rootfsmy/
cp led1 ~/rootfsmy/
# Clean modules and application
make arch=arm clean
rm led1