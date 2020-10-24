//client
#include "header.h"

int rsfd;

int main(int argc, char **argv)
{
    printf("\n+-----------+");
    printf("\n|     C     |");
    printf("\n+-----------+\n");
    printf("Will also send messages to B by pretending to be A\n");

    if ((rsfd  = socket(AF_INET,SOCK_RAW,150)) < 0)        //any protocol no above 255 is not taken
		error("error in socket()");
    cout << "Socket created.\n";

    int optval = 1;
    setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

    //set addresses
    struct sockaddr_in c_addr = setAddress(IP_C);
	struct sockaddr_in b_addr = setAddress(IP_B);

    if (bind(rsfd,(sockaddr*)&c_addr,sizeof(c_addr)) < 0)
        error("bind()");
    cout << "Address bound.\n";

	//make IP header
	char packet[PACKET_LEN];
	memset(packet, 0, sizeof(packet));
	struct ip* hdr = makeIPHeader(150, IP_C, IP_B, packet);

    //loop
    const char *signature = " - from C";
    int alreadySpoofed = 0;
    printf("\nSending with C's IP - won't work. \nTry, then enter 'spoof' to change to A's IP.\n");
	while (1) {
        cout << "Enter: ";
        char buf[MAXLEN];
        clear(buf);
        scanf("%s", buf);

        //change header details
        if (strcmp(buf, "spoof") == 0) {
            if (alreadySpoofed)
                printf("Already spoofed!\n");
            else {
                struct in_addr a_addr;
                a_addr.s_addr = inet_addr(IP_A);
                hdr->ip_src = a_addr;
                printf("\nChanged IP: You can now send to B :)\n");
            }
            continue;
        }

        //sendto() is non blocking
        strcat(buf, signature);
        strcpy(packet+MSG_OFFSET, buf);
        printf("Sending: '%s'...\n", packet+(MSG_OFFSET));
        if (sendto(rsfd,&packet,sizeof(packet),0,(sockaddr*)&b_addr,sizeof(b_addr)) < 0)
              error("sendto");
	}

	return 0;
}
