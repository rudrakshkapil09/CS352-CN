//Server Side - will write to file
#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<pcap.h>
#include<errno.h>
#include <netdb.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/udp.h>

#define ADDRESS "mysocket"
#define MSG_CMSG_CLOEXEC 0x40000000
#define PORT 8080
#define	MAXLEN 1024

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

//
//NORMAL SOCKET STUFF
//

//clearing
void clear(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
        str[i] = '\0';
}

//error printing utility function
void error (char *str) {
	perror(str);
	exit(-1);
}

//global socket file descriptor - original 'table'
int sfd;

//utility function to close the socket
void cleanup(int signo)
{
	printf("Cleaning up sockets...\n");
	shutdown(sfd, SHUT_RDWR);	//disallow further reads & writes
	close(sfd);
	printf("Cleaned sockets!\n");
	exit(0);
}

//MAIN
int main() {
    printf("+--------------------------+\n");
	printf("|      	   SENDER          |\n");
    printf("|  (type 'done' to finish) |\n");
	printf("+--------------------------+\n");

    //FIRST CONNECT TO SERVER AND USE SERVICES
    //when program is terminated, clean up socket
	signal(SIGINT, cleanup);

	//open socket
	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Error in socket()\n");
	else printf("Successfully set up socket.\n");

	//get host name - because ipv4 is specified, function copies name into the h_name field
	//and its struct in_addr equivalent into the h_addr_list[0] field of the returned hostent struct
	struct hostent *hp;
	hp = gethostbyname("127.0.0.1");
	if (hp == NULL)
		error("Error in gethostname()\n");
	else printf("Got hostname.\n");

	//setting up address
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);

	//copying hostname hp->h_addr into address.sin_addr
	bcopy(hp->h_addr, (char *) &address.sin_addr, hp->h_length);	//returns NULL
	printf("Successfully finished byte copy of hostname.\n");

	//address we specify is the address of server we would like to comm. with
	if ((connect(sfd, (struct sockaddr*) &address, sizeof(address))) < 0)
		error("Error in connect()\n");

	//continuously send and receive
	char buff[MAXLEN];
	while (1) {
		//send message
        clear(buff);
		scanf("%s", buff);

        if (strcmp(buff, "done") == 0)
            break;

		if (send(sfd, buff, strlen(buff), 0) < 0)
			error("Error in send()\n");

		//receive message
        clear(buff);
		while (1) {
			int n;
			char msg[25];
			if ((n = recv(sfd, &msg, sizeof(msg), 0)) < 0)
				error("Error in recv()\n");
			else if (n > 0) {
				printf("Recieved => %s\n", msg);
				break;
			}
		}
	}

    //THEN PASS SFD TO RECEIVER
    printf("Waiting for receiver...\n");
	int  usfd;
	struct sockaddr_un userv_addr, ucli_addr;
  	int userv_len, ucli_len;


    //socket
	if ((usfd = socket(AF_UNIX , SOCK_STREAM , 0)) == -1)
	   perror("socket");

    //set address
  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
	strcpy(userv_addr.sun_path, ADDRESS);
	unlink(ADDRESS);

    //bind and listen
    userv_len = sizeof(userv_addr);
	if (bind(usfd, (struct sockaddr *)&userv_addr, userv_len) == -1)
		perror("server: bind");
	listen(usfd, 5);


    //accept connections
	int nusfd;
    ucli_len = sizeof(ucli_addr);
	nusfd = accept(usfd, (struct sockaddr *) &ucli_addr, (socklen_t *) &ucli_len);
    printf("Accepted client!\n");

    //send fd to receiver
    printf("Sending %d...\n", sfd);
    send_fd(nusfd, sfd);

    //done:
    printf("Done!\n");
    close(nusfd);
    close(usfd);
    close(sfd);
}
