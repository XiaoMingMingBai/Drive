/**
 * This file is used to open a device driver file and call read/write and close.
*/
#include <head.h>
int main(int argc,const char * argv[])
{
    int fd;
    char s[128];
    if((fd = open("/dev/mycdev",O_RDWR))==-1)
        PRINT_ERR("open error");
    write(fd,s,sizeof(s));
    read(fd,s,sizeof(s));
    close(fd);
    return 0;
}