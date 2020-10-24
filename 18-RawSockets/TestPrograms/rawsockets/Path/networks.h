#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <string.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct MyIPheader
{
	#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ihl:4;
    unsigned int version:4;
	#elif __BYTE_ORDER == __BIG_ENDIAN
	    unsigned int version:4;
	    unsigned int ihl:4;
	#else
	# error	"Please fix <bits/endian.h>"
	#endif
	    uint8_t tos;
	    uint16_t tot_len;
	    uint16_t id;
	    uint16_t frag_off;
	    uint8_t ttl;
	    uint8_t protocol;
	    uint16_t check;
	    uint32_t saddr;
	    uint32_t daddr;
	char name_of_sender[10];
};
