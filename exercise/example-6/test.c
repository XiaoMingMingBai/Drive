#include <head.h>

int main(int argc, char const *argv[])
{
    int fd;
    char s[128] = {0};
    if ((fd = open("/dev/myled", O_RDWR)) == -1)
    {
        PRINT_ERR("open error\n");
    }
    while (1)
    {
        s[0] = !s[0];
        write(fd, s, sizeof(s));
        sleep(1);
    }
    close(fd);
    return 0;
}
