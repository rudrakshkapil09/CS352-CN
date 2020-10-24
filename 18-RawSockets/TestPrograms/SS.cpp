#include"header.h"
int main()
{
    int nsfd;
    int usfd=createUXSocket("Unix2");
    sockaddr_un usaddr;
    sockaddr saddr;
    socklen_t addrlen;
    setAddress(usaddr,"Unix1");
    int p=connect(usfd,(sa) &usaddr,sizeof(usaddr));
    if(p<0)
    perror("Connect usfd error\n");
    int sfd;
    char Buf[1];
    sock_fd_read(usfd,Buf,1,&sfd);
    sock_fd_read(usfd,Buf,1,&nsfd);
    cout<<"Socket received from other program\n";
    int nsfd2=accept(sfd,&saddr,&addrlen);
    cout<<"Accepted second client\n";
    int c=0;
    
    while(true)
    {
    	if(c%2){
        char Buffer[1024];
		memset(Buffer,'\0',sizeof(Buffer));
        recv(nsfd,Buffer,1024,0);
        strcat(Buffer,"123");
        send(nsfd,Buffer,strlen(Buffer)+1,0);
        }
        else
        {
            char Buffer[1024];
            memset(Buffer,'\0',sizeof(Buffer));
	    recv(nsfd2,Buffer,1024,0);
	    cout<<"Received buffer "<<Buffer<<"-\n";
	    strcat(Buffer,"123");
	    send(nsfd2,Buffer,strlen(Buffer)+1,0);
        }
        c++;
    }
}
