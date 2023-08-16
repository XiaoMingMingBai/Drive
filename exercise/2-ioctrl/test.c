#include <head.h>
#include <sys/ioctl.h>
#include "myled.h"
void led_blink(int fd, int which)
{
    ioctl(fd, LED_ON, &which);
    sleep(1);
    ioctl(fd, LED_OFF, &which);
    sleep(1);
}
int main(int argc, const char *argv[])
{
    int fd;

    if ((fd = open("/dev/myled", O_RDWR)) == -1)
        PRINT_ERR("open error");
    while (1)
    {
        led_blink(fd, LED1);
        led_blink(fd, LED2);
        led_blink(fd, LED3);
    }
    close(fd);
    return 0;
}