#include <head.h>
#include <sys/ioctl.h>
#include "lightled.h"
void led_blink(int fd, int which)
{
    ioctl(fd, LED_ON, &which);
    sleep(1);
    ioctl(fd, LED_OFF, &which);
    sleep(1);
}
int main(int argc, const char* argv[])
{
    int fd;
    if ((fd = open("/dev/myleds", O_RDWR)) == -1)
        PRINT_ERR("open error");
    while (1) {
        for (int i = 0; i < 6; i++) {
            led_blink(fd, i);
        }
    }
    close(fd);
    return 0;
}