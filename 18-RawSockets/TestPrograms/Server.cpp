#include"header.h"
int main()
{
    int sfd=createTCPSocket(9091,3);
    int usfd=createUXSocket("Unix1",1);
    sockaddr usaddr;
    socklen_t uaddrlen;
    int nusfd=accept(usfd,&usaddr,&uaddrlen);
    int nsfd=accept(sfd,&usaddr,&uaddrlen);
    char Buffer[1024]="";
    recv(nsfd,Buffer,1024,0);
    strcat(Buffer,"123");
    send(nsfd,Buffer,strlen(Buffer)+1,0);
    sock_fd_write(nusfd,(void *)"1",1,sfd);
    sock_fd_write(nusfd,(void *)"1",1,nsfd);
    cout<<"Socket sent to other program\n";
    sleep(3);
    //close(nsfd);
    //close(sfd);
    while(1);
}
