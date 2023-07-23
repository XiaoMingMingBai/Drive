/**
 * This file is used to test the driver's open/read/write/close function
*/
#include <head.h>

int main(int argc, char const *argv[])
{
    int fd;
    char s[128] = "This is test data ...\n";
    if ((fd = open("/dev/mycdev", O_RDWR)) == -1)
    {
        PRINT_ERR("open error\n");
    }
    write(fd, s, sizeof(s));
    memset(s, 0, sizeof(s));
    read(fd, s, sizeof(s));
    printf("fd : %s\n", s);
    close(fd);
    return 0;
}
