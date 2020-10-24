#include"header.h"
#include"ip_icmp.h"

#define pckt_size 64
#define PING_SLEEP_RATE 1000000

int sig=0;

unsigned short checksum(void *b, int len){
	unsigned short *buf = b;
	unsigned int sum=0;
	unsigned short result;

	for(sum=0;len>1;len-=2)
		sum+=*buf++;
	if(len==1)
		sum+=*(unsigned char*)buf;
	sum=(sum>>16)+(sum&0xFFFF);
	sum+=(sum>>16);
	result=~sum;
	return result;
}

void hdfn(){
	sig=1;
}

struct packet{
	struct icmphdr hdr;
	char msg[pckt_size-sizeof(struct icmphdr)];
};

void send_ping(int rsfd,struct sockaddr_in serv_addr,char *c,char *buf){
	struct packet pckt;
	int ttl=64, msg_count=0,i,addr_len,flag=1,msg_received_count=0;
     	struct sockaddr_in recv_addr;
     	addr_len=sizeof(recv_addr);
    	struct timespec time_start, time_end, start, end;
    	long double rtt_msec=0, total_msec=0,min_rtt=100000,avg_rtt=0,max_rtt=-1;
    	struct timeval timeout;
    	timeout.tv_sec=1;
    	timeout.tv_usec=0;

    	clock_gettime(CLOCK_MONOTONIC,&start);

    	if(setsockopt(rsfd,IPPROTO_IP,IP_TTL,&ttl, sizeof(ttl))<0) perror("ttl");

    	if(setsockopt(rsfd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout, sizeof timeout)<0) perror("rcvtimeo");

    	while(1){
    		if(sig==1)
    			break;
    		bzero(&pckt,sizeof(pckt));
    		pckt.hdr.type=ICMP_ECHO;
    		pckt.hdr.un.echo.id=getpid();
    		for (i=0;i<sizeof(pckt.msg)-1;i++)
            		pckt.msg[i] = i+'0';
 		pckt.msg[i] = 0;
    		pckt.hdr.un.echo.sequence=msg_count++;
    		pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
 		usleep(PING_SLEEP_RATE);
    		clock_gettime(CLOCK_MONOTONIC,&time_start);
    		if(sendto(rsfd,&pckt,sizeof(pckt),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){ perror("send failed");flag=0; }
    		if(recvfrom(rsfd,&pckt,sizeof(pckt),0,(struct sockaddr*)&recv_addr,(socklen_t*)&addr_len)<=0)	perror("receive failed");
    		else{
	    		clock_gettime(CLOCK_MONOTONIC,&time_end);
	    		double timeElapsed=((double)(time_end.tv_nsec-time_start.tv_nsec))/1000000.0;
		    	rtt_msec=(time_end.tv_sec-time_start.tv_sec)*1000.0+timeElapsed;
		    	if(rtt_msec<min_rtt)
		    		min_rtt=rtt_msec;
		    	if(rtt_msec>max_rtt)
		    		max_rtt=rtt_msec;
		    	avg_rtt+=rtt_msec;
	    		if(flag){
	    			printf("%d bytes from %s (%s):\n icmp_seq=%d ttl=%d time=%.3Lf ms\n",pckt_size,buf,inet_ntoa(*(struct in_addr*)&serv_addr.sin_addr),msg_count,ttl,rtt_msec);
	    			msg_received_count++;
	    		}
	    	}
    	}

    	clock_gettime(CLOCK_MONOTONIC,&end);
    	double timeElapsed=((double)(end.tv_sec-start.tv_sec))/1000000.0;
        total_msec=(end.tv_sec-start.tv_sec)*1000.0+timeElapsed;

        printf("\n--- %s ping statistics ---\n",c);
        printf("%d packets transmitted, %d received, %.0f%% packet loss, time %.0Lfms\n",msg_count,msg_received_count,(double)((msg_count-msg_received_count)/msg_count)*100,total_msec);
        printf("rtt min/avg/max = %.3Lf/%.3Lf/%.3Lf ms\n",min_rtt,max_rtt,avg_rtt/msg_received_count);
}

int main(int argc,char *argv[]){
	if(argc<2){
		printf("\nrequired format : %s <ip address or host name>\n",argv[0]);
		exit(0);
	}
	struct sockaddr_in serv_addr;
	struct hostent *host_entity=gethostbyname(argv[1]);
	serv_addr.sin_family=host_entity->h_addrtype;
	serv_addr.sin_addr.s_addr=*(long*)host_entity->h_addr;

	char buf[NI_MAXHOST];
	getnameinfo((struct sockaddr*)&serv_addr,sizeof(serv_addr),buf,sizeof(buf),NULL,0,NI_NAMEREQD);
	int rsfd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
	struct sockaddr_in myaddr;
	myaddr.sin_family=AF_INET;
	myaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(rsfd,(struct sockaddr*)&myaddr,sizeof(myaddr))<0) perror("bind");
	if(rsfd<0) perror("raw socket");
	signal(SIGINT,hdfn);

	printf("PING %s (%s) 56(84) bytes of data.\n",argv[1],inet_ntoa(*(struct in_addr*)&serv_addr.sin_addr));
	send_ping(rsfd,serv_addr,argv[1],buf);
	return 0;
}
