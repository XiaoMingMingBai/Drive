#include <head.h>
#include "myled.h"

void led_blink(int fd, enum led_num which)
{
    char s[2] = { which, ON};
    write(fd, s, sizeof(s));
    usleep(600000);
    s[1] = OFF;
    write(fd, s, sizeof(s));
}
int main(int argc, char const *argv[])
{
    int fd;
    if ((fd = open("/dev/myled", O_RDWR)) == -1)
    {
        PRINT_ERR("open error\n");
    }
    while (1)
    {
        led_blink(fd, LED1);
        led_blink(fd, LED2);
        led_blink(fd, LED3);
    }
    
    return 0;
}
