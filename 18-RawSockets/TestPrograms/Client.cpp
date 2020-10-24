#include"header.h"
int main(int argc, char * argv[])
{
	if(argc!=2)
		printf("\n usage ./a.out port_no");

		int sfd;
		struct sockaddr_in serv_addr;
		int port_no=atoi(argv[1]);
		char buffer[256];

		bzero(&serv_addr,sizeof(serv_addr));

		if((sfd = socket(AF_INET , SOCK_DGRAM , 0))==-1)
		perror("\n socket");
		else printf("\n socket created successfully\n");
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(port_no);
		inet_aton("127.255.255.255", &serv_addr.sin_addr);
		socklen_t serv_len = sizeof(serv_addr);
		int opt=1;
		setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));
		int p=bind(sfd,(sa) &serv_addr,serv_len);
		if(p<0)
		perror("Bind error\n");
		//fgets( buffer , 256 , stdin );
		//sendto(sfd , buffer , 256 , 0 , ( struct sockaddr * ) &serv_addr ,  serv_len);
		recvfrom(sfd , buffer , 256 , 0 , ( struct sockaddr * ) &serv_addr , & serv_len );
		cout<<buffer<<"\n";
}

