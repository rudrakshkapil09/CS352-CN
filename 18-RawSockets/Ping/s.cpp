#include "header.h"
#define PORT_NO 0
#define PING_SLEEP_RATE 1000000

int rsfd, done = 0;

void hdfn(int signo)
{
	done = 1;
}

// Calculating the Check Sum
unsigned short checksum(void *b, int len)
{
	unsigned short *buf = (unsigned short *) b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

//packet
struct packet {
	struct icmphdr hdr;
	char msg[PACKET_LEN-sizeof(struct icmphdr)];
};

//send ping
void send_ping(struct sockaddr_in serv_addr, char *c)
{
	struct packet pckt;							//to be sent
	int msg_count = 0, msg_received_count = 0;	//count of transmitted and received

	//time variables
	struct timespec time_start, time_end, start, end;
	long double rtt_msec = 0, total_msec = 0, min_rtt = INT_MAX, avg_rtt = 0, max_rtt = INT_MIN;

	//address stuff for recv
 	struct sockaddr_in recv_addr;
 	int addr_len = sizeof(recv_addr);

	//get starting time for all
	clock_gettime(CLOCK_MONOTONIC, &start);

	//set TTL for the packet
	int ttl = 64;
	if (setsockopt(rsfd,IPPROTO_IP,IP_TTL,&ttl, sizeof(ttl)) < 0)
		error("ttl");

	//set timeout of one second for input operation
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	if (setsockopt(rsfd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout, sizeof timeout) < 0)
		error("rcvtimeo");

	//breaks when ^C is pressed
	int failure_count = 0;
	while (!done) {
		int sent = 1;

		//icmp header info
		int i = 0;
		bzero(&pckt, sizeof(pckt));
		pckt.hdr.type = ICMP_ECHO;			//type is echo
		pckt.hdr.un.echo.id = getpid();
		for (; i < sizeof(pckt.msg)-1; i++)
        	pckt.msg[i] = i+'0';						//put in a message
		pckt.msg[i] = 0;
		pckt.hdr.un.echo.sequence = msg_count++;		//store sequence number
		pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));

		usleep(PING_SLEEP_RATE);										//suspend execution for 1 second
		clock_gettime(CLOCK_MONOTONIC, &time_start);	//get start time for this packet

		//send and then...
		if (sendto(rsfd,&pckt,sizeof(pckt),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
			error("sendto()");
			sent = 0;
		}

		//wait for receive
		if (recvfrom(rsfd,&pckt,sizeof(pckt),0,(struct sockaddr*)&recv_addr,(socklen_t*)&addr_len) < 0) {
			failure_count++;
			printf("Failed to get packet!\n");
			if (failure_count >= 5)
				error("Too many failed receive attempts :(\n");
			continue;
		}

		//get end time and increment counts
		clock_gettime(CLOCK_MONOTONIC, &time_end);
		double timeElapsed = ((double)(time_end.tv_nsec-time_start.tv_nsec))/1000000.0;
    	rtt_msec = (time_end.tv_sec-time_start.tv_sec)*1000.0 + timeElapsed;

		if (sent) {
	    	if (rtt_msec < min_rtt)
	    		min_rtt = rtt_msec;
	    	if (rtt_msec > max_rtt)
	    		max_rtt = rtt_msec;
	    	avg_rtt += rtt_msec;

			printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3Lf ms\n",PACKET_LEN,inet_ntoa(*(struct in_addr*)&serv_addr.sin_addr),msg_count,ttl,rtt_msec);
			msg_received_count++;
		}
	}

	//get ending time for all
	clock_gettime(CLOCK_MONOTONIC, &end);
	double timeElapsed = ((double)(end.tv_sec-start.tv_sec))/1000000.0;
    total_msec = (end.tv_sec-start.tv_sec)*1000.0 + timeElapsed;

	//calculate overall statistics
    printf("\n--- %s ping statistics ---\n", c);
    printf("%d packets transmitted, %d packets received, %.0f%% packet loss, time %.0Lfms\n",msg_count,msg_received_count,(double)((msg_count-msg_received_count)/msg_count)*100,total_msec);
    printf("round-trip min/max/avg = %.3Lf/%.3Lf/%.3Lf ms\n",min_rtt,max_rtt,avg_rtt/msg_received_count);
}

//MAIN
int main(int args, char *argv[])
{
	signal(SIGINT, hdfn);
	if (args < 2)
		error("Enter IP!");

	printf("\n+-----------+");
	printf("\n|    PING   |");
	printf("\n+-----------+\n");

	//filling up address structure
	struct sockaddr_in serv_addr;
	struct hostent *host_entity = gethostbyname(argv[1]);

	serv_addr.sin_family = host_entity->h_addrtype;
	serv_addr.sin_port = htons (PORT_NO); 			// 0
	serv_addr.sin_addr.s_addr = *(long*) host_entity->h_addr;

	//call socket and bind
	if ((rsfd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP)) < 0)        //any protocol no above 255 is not taken
		error("error in socket()");
	cout << "Socket created.\n";

	struct sockaddr_in this_addr;
	this_addr.sin_family = AF_INET;
	this_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(rsfd,(struct sockaddr*)&this_addr,sizeof(this_addr)) < 0)
		error("error in bind()");
	cout << "Address Bound!\n";

	//send packets
	printf("PING %s (%s): 56 data bytes\n",argv[1],inet_ntoa(*(struct in_addr*)&serv_addr.sin_addr));
	send_ping(serv_addr, argv[1]);
	return 0;
}
