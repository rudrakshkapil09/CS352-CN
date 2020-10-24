//server
#include "header.h"

int main()
{
	system("g++ s_tcp.cpp -o s_tcp");
	system("g++ c_tcp.cpp -o c_tcp");
	system("g++ s_udp.cpp -o s_udp");
	system("g++ c_udp.cpp -o c_udp");
}
