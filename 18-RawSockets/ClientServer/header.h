//Header file with include statements and utility functions
#include <time.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/un.h>
#include <pcap.h>
#include <poll.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <assert.h>
#include <sys/msg.h>
#include <assert.h>
using namespace std;
extern int errno;

//Ports and addresses
#define F_PORT 7000
#define IP_SERVER "127.0.0.1"
#define IP_CLIENT "127.0.0.2"

//other constants
#define MSG_CMSG_CLOEXEC 0x40000000
#define MAXLEN 100
#define MAX_PENDING 10


//SPECIFIC FOR THIS:



/*-------------------structs--------------------*/
struct sembuf p = { 0, -1, SEM_UNDO};   // WAIT
struct sembuf v = { 0, +1, SEM_UNDO};   // SIGNAL
/*----------------------------------------------*/

/*-------------------utility functions----------------*/
//utility for maxx
int max(int x, int y)
{
    return (x > y) ? x : y;
}

//utility for gnome basically
void openNewTerminal(char *program)
{
    char cmd[200], path[100];
    sprintf(cmd, "osascript -e 'tell app \"Terminal\" to do script \"%s/%s\"'", getcwd(path, 100), program);
    system(cmd);
}

//error printing utility function
void error (const char *str) {
    int errnum = errno & EADDRNOTAVAIL;
    fprintf(stderr, "Value of errno: %d\n", errnum);
	perror(str);
	exit(-1);
    //ssssprintf("%s\n", str);
}

//utility to clear buffer
void clear(char buff[])
{
	for (int i = 0; i < MAXLEN; i++)
		buff[i] = '\0';
}



//Wait
void Wait(int semid, int semindex)
{
	p.sem_num = semindex;
	semop(semid, &p, 1);
}

//Signal
void Signal(int semid, int semindex)
{
	v.sem_num = semindex;
	semop(semid, &v, 1);
}

//used to set address of process
void setAddress(struct sockaddr_in *address, int portNum)
{
    (*address).sin_family = AF_INET;
    (*address).sin_port = htons(portNum);
    (*address).sin_addr.s_addr = INADDR_ANY;
}

//utility to create dgram socket - server
struct sockaddr_in createSocketDG(int *sfd, int port)
{
    //socket()
    if (((*sfd) = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        error("Error in socket()");

    //set address
    struct sockaddr_in address;
    setAddress(&address, port);

    //set reusable
    int trued = 1;
    setsockopt((*sfd), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &trued, sizeof(true));

    //Bind
    if (bind((*sfd), (struct sockaddr *) &address, sizeof(address)) < 0)
        error("Error in bind()");

    //return
    printf("Socket set up!\n");
    return address;
}

//creates socket and returns that sfd - server
struct sockaddr_in createSocket(int *sfd, int port)
{
	//open socket
	if (((*sfd) = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error in socket()\n");
	//printf("Successfully set up socket.\n");

	//setting up socket address
    struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons((uint16_t) port);	//returns 32 bit integer in host byte order
	address.sin_addr.s_addr = INADDR_ANY;

	//VVIP STEP makes socket reuseble
	int trued = 1;
	setsockopt((*sfd), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &trued, sizeof(true));

	//binding address to server
	if (bind((*sfd), (struct sockaddr*) &address, sizeof(address)) < 0) {
		if (shutdown((*sfd), SHUT_RDWR) < 0)
			error("Error in shutdown()\n");
		close((*sfd));
		error("Error in bind()\n");
	}
	//printf("Address successfully binded to socket.\n");

	//listening to client requests
	if ((listen((*sfd), MAX_PENDING)) < 0)
		error("Error in listen()\n");
	//printf("listen() successful.\n");

    printf("Socket set up!\n");
	return address;
}

//-------------- UNIX SOCKETS --------------------//

//to send a fd
int send_fd(int socket, int fd_to_send)
{
    //vars
    struct msghdr socket_message;   // will be sent
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];

    // storage space needed for an ancillary element with a payload of length is CMSG_SPACE(sizeof(length))
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
    int available_ancillary_element_buffer_space;

    // at least one vector of one byte must be sent
    message_buffer[0] = 'F';
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;

    // initialize socket message
    memset(&socket_message, 0, sizeof(struct msghdr));
    socket_message.msg_iov = io_vector;     // data blocks
    socket_message.msg_iovlen = 1;          // number of blocks

    // provide space for the ancillary data
    available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
    memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
    socket_message.msg_control = ancillary_element_buffer;                      //
    socket_message.msg_controllen = available_ancillary_element_buffer_space;   // length of csmg list

    // initialize a single ancillary data element for fd passing
    control_message = CMSG_FIRSTHDR(&socket_message);   //returns pointer to first cmsghdr in the ancillary data buffer associated with socket_message
    control_message->cmsg_level = SOL_SOCKET;           // originating protocol
    control_message->cmsg_type = SCM_RIGHTS;            // protocol specific type - VVIMP
    control_message->cmsg_len = CMSG_LEN(sizeof(int));  //data byte count
    *((int *) CMSG_DATA(control_message)) = fd_to_send; //returns pointer to the data portion of control_message cmsghdr

    // send fd
    return sendmsg(socket, &socket_message, 0);
}


//receive fd
int recv_fd(int socket)
{
    // vars
    int sent_fd, available_ancillary_element_buffer_space;
    struct msghdr socket_message;
    struct iovec io_vector[1];
    struct cmsghdr *control_message = NULL;
    char message_buffer[1];
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

    // start clean
    memset(&socket_message, 0, sizeof(struct msghdr));
    memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

    // setup a place to fill in message contents
    io_vector[0].iov_base = message_buffer;
    io_vector[0].iov_len = 1;
    socket_message.msg_iov = io_vector;
    socket_message.msg_iovlen = 1;

    // provide space for the ancillary data
    socket_message.msg_control = ancillary_element_buffer;
    socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

    // get message
    if (recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
        return -1;

    /* this did not originate from the above function */
    if (message_buffer[0] != 'F')
        return -1;

    /* we did not provide enough space for the ancillary element array */
    if ((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
        return -1;

    // iterate ancillary elements
    for(control_message = CMSG_FIRSTHDR(&socket_message);
       control_message != NULL;
       control_message = CMSG_NXTHDR(&socket_message, control_message))
    {
        if ((control_message->cmsg_level == SOL_SOCKET) &&
            (control_message->cmsg_type == SCM_RIGHTS))
            {
                sent_fd = *((int *) CMSG_DATA(control_message));
                return sent_fd;
            }
    }

    //none received
    return -1;
}

//------------------ RAW SOCKETS -----------------------//

/*
struct ip {
#if BYTE_ORDER == LITTLE_ENDIAN
    u_char  ip_hl:4,                // header length
        ip_v:4;                     // version
#endif
#if BYTE_ORDER == BIG_ENDIAN
    u_char  ip_v:4,                 // version
        ip_hl:4;                    // header length
#endif
    u_char  ip_tos;                 // type of service
    short   ip_len;                 // total length
    u_short ip_id;                  // identification
    short   ip_off;                 // fragment offset field
#define IP_DF 0x4000                // dont fragment flag
#define IP_MF 0x2000                // more fragments flag
    u_char  ip_ttl;                 // time to live
    u_char  ip_p;                   // protocol
    u_short ip_sum;                 // checksum
    struct  in_addr ip_src,ip_dst;  // source and dest address
};
*/

void printIPH(struct ip* hdr)
{
    printf("\n\tIP HEADER:\n");
    printf("+--------+--------+----------------+----------------------------------+\n");
    printf("| {VERS} | {HLEN} |      {TOS}     |          {TOTAL LENGTH}          |\n");
    printf("|%5d   |%5d   |%9d       |%18d                |\n",hdr->ip_v,hdr->ip_hl,hdr->ip_tos,hdr->ip_len);
    printf("+--------+--------+----------------+---+---+---+----------------------+\n");
    printf("|         {IDENTIFICATION}         |{X}|{D}|{M}|       {OFFSET}       |\n");
    printf("|%20d              | - | - | - |%13d         |\n", hdr->ip_id, hdr->ip_off);
    printf("+-----------------+----------------+---+---+---+----------------------+\n");
    printf("|      {TTL}      |   {PROTOCOL}   |            {CHECKSUM}            |\n");
    printf("|%9d        |%9d       |%18d                |\n", hdr->ip_ttl, hdr->ip_p, hdr->ip_sum);
    printf("+-----------------+----------------+----------------------------------+\n");
    printf("|                         {SOURCE IP ADDRESS}                         |\n");
    printf("|%42s                           |\n", inet_ntoa(hdr->ip_src));
    printf("+---------------------------------------------------------------------+\n");
    printf("|                       {DESTINATION IP ADDRESS}                      |\n");
    printf("|%42s                           |\n", inet_ntoa(hdr->ip_dst));
    printf("+---------------------------------------------------------------------+\n");
    printf("\n");
}
