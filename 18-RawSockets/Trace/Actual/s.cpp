#include "header.h"
#define PORT_NO 0
#define PING_SLEEP_RATE 1000000

int rsfd, done = 0;

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

//send ping
void send_ping(struct sockaddr_in serv_addr)
{
	//vars
	int hop = 1;
	char packet[64] = {0};
	struct ip * ip_hdr;

	//address stuff for recv
 	struct sockaddr_in recv_addr;
 	int addr_len = sizeof(recv_addr);

	//we will create a header for each packet
	int optval = 1;
    setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

	//breaks when ^C is pressed - increment hop each time to see how far it reached
	while (!done) {
		//create IP header
		ip_hdr = makeIPHeader(IPPROTO_ICMP, "172.30.231.152", "127.0.0.2", packet);
		ip_hdr->ip_ttl = hop;
		ip_hdr->ip_len = 20 + 8;
		ip_hdr->ip_sum = checksum((unsigned short *)packet, 9);
		inet_pton (AF_INET, inet_ntoa(serv_addr.sin_addr), &(ip_hdr->ip_dst));

		//create ICMP header
		struct icmphdr * icmp_hdr = (struct icmphdr *) (packet + 20);
		icmp_hdr->type = ICMP_ECHO;
		icmp_hdr->code = 0;
		icmp_hdr->un.echo.id = 0;
		icmp_hdr->un.echo.sequence = hop + 1;
		icmp_hdr->checksum = checksum ((unsigned short *) (packet + 20), 4);

		//send packet and receive
		if (sendto(rsfd,packet,sizeof(struct ip) + sizeof(struct icmphdr),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
			error("sendto()");

		struct sockaddr_in recv_addr;
		int addr_len = sizeof(recv_addr);
		if (recvfrom(rsfd,packet,sizeof(packet),0,(struct sockaddr*)&recv_addr,(socklen_t*)&addr_len) < 0)
			error("recvfrom()");

		//extract the received ICMP header
		icmp_hdr = (struct icmphdr *) (packet + 20);
		printf("[%2d] - %s\n", (hop), inet_ntoa(recv_addr.sin_addr));

		

		if (icmp_hdr->type == 0) {
			printf("\nReached Destination!\n");
			break;
		}
		hop++;
	}

}

//MAIN
int main(int args, char *argv[])
{
	if (args < 2)
		error("Enter IP!");

	printf("\n+------------+");
	printf("\n|    TRACE   |");
	printf("\n+------------+\n");

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
	printf("Traceroute has started...\n");
	printf("traceroute to %s (%s): 64 hops max, 72 byte packets\n",argv[1],inet_ntoa(*(struct in_addr*)&serv_addr.sin_addr));
	send_ping(serv_addr);
	return 0;
}
