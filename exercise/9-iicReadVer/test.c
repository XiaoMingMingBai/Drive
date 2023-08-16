#include "si7006.h"
#include <head.h>
#include <sys/ioctl.h>

int main(int argc, char const *argv[])
{
    int fd;
    int serial, firmware;
    if ((fd = open("/dev/si7006", O_RDWR)) == -1)
    {
        perror("open");
        exit(1);
    }
    if (ioctl(fd, GET_SERIAL, &serial) == -1)
    {
        perror("ioctl");
        exit(1);
    }
    if (ioctl(fd, GET_FIRMWARE, &firmware) == -1)
    {
        perror("ioctl");
        exit(1);
    }
    printf("serial: %#x, firmware: %#x\n", serial, firmware);
    close(fd);
    return 0;
}
