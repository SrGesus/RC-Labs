#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
int main(void)
{
    int fd;
    struct sockaddr_in addr;
    socklen_t addrlen;
    ssize_t n;
    char buffer[128];

    fd=socket(AF_INET,SOCK_DGRAM,0);
    if (fd==-1)
        exit(1);

    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
    if (n==-1)
        exit(1);

    write(1,"echo: ",6);
    write(1,buffer,n);
    close(fd);

    exit(0);
}