#include"header.h"
int main()
{
    int sfd=socket(AF_INET,SOCK_DGRAM,0);
    int opt=1;
    //setsockopt(sfd,SOL_SOCKET,SO_BROADCAST,&opt,sizeof(opt));
    sockaddr_in saddr;
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(8081);
    inet_aton("127.255.255.255",&saddr.sin_addr);
    int x=sendto(sfd,"Hello",6,0,(sa)&saddr,sizeof(saddr));
    if(x<0)
    perror("SEnd error\n");
}
