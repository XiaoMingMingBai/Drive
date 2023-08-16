#include "iichumiture.h"
#include <head.h>
#include <sys/ioctl.h>

int main(int argc, char const *argv[])
{
    int fd;
    int serial, firmware;
    int hum, tmp;
    float rhum, rtmp;
    if ((fd = open("/dev/si7006", O_RDWR)) == -1)
    {
        perror("open");
        exit(1);
    }

    if (ioctl(fd, GET_SERIAL, &serial) == -1)
        PRINT_ERR("ioctl error");
    if (ioctl(fd, GET_FIRMWARE, &firmware) == -1)
        PRINT_ERR("ioctl error");
    printf("serial = %#x,firmware = %#x\n", serial, firmware);
    
    while (1)
    {
        ioctl(fd, GET_TMP, &tmp);
        ioctl(fd, GET_HUM, &hum);

        rtmp = 175.72 * tmp / 65536 - 46.85;
        rhum = 125.0 * hum / 65536 - 6;

        printf("rtmp = %.2f, rhum = %.2f\n", rtmp, rhum);
        sleep(1);
    }
    close(fd);
    return 0;
}
