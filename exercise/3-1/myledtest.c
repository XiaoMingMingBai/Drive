#include <head.h>

int main(int argc,const char * argv[])
{
    int fd;
    pid_t pid;
    char s[128] = "i am test block IO model...\n";
    if((fd = open("/dev/mycdev0",O_RDWR))==-1)
        PRINT_ERR("open error");
    
    pid = fork();
    if(pid == -1){
        PRINT_ERR("fork error");
    }else if(pid == 0){
        // 子进程 读
        while(1){
            memset(s,0,sizeof(s));
            read(fd,s,sizeof(s));
            printf("s = %s\n",s);
        }
    }else{
        // 父进程 写
        while(1){
            sleep(3);
            write(fd,s,sizeof(s));
        }
    }
    return 0;
}