#include <sys/ioctl.h>
#include <head.h>
#include "ioctrlStruct.h"
void led_blink(int fd, int which)
{
    ioctl(fd,LED_ON,&which);
    sleep(1);
    ioctl(fd,LED_OFF,&which);
    sleep(1);
}
int main(int argc, const char* argv[])
{
    int fd;
    char s[128] = "this is test ioctl uaccess char [128]";
    img_t img = {1920,1080};
    if ((fd = open("/dev/myled", O_RDWR)) == -1)
        PRINT_ERR("open error");

    ioctl(fd, UACCESS_CHAR_ARRAY, s);
    ioctl(fd, UACCESS_STRUCT, &img);
    printf("img.width = %d,img.high = %d\n", img.width, img.high);

    while (1) {
        led_blink(fd, LED1);
        led_blink(fd, LED2);
        led_blink(fd, LED3);
    }
    close(fd);
    return 0;
}