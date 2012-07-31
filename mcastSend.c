#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int sendfd;
	socklen_t salen;
	struct sockaddr_in addr;
	int ipaddr;

	sendfd = socket(AF_INET, SOCK_DGRAM, 0);
	if ( sendfd < 0 )
	{
		perror("opening datagram socket");
		exit(1);
	}

	addr.sin_family = AF_INET;	
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = inet_addr("239.255.1.2");

	// ipaddr = inet_addr("127.0.0.1");
	ipaddr = INADDR_ANY;
	if ( setsockopt(sendfd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&ipaddr, sizeof(ipaddr)) != 0 )
	{
		perror("setsockopt IP_MULTICAST_IF error");
		close(sendfd);
		exit(1);
	}

	{
		char line[256];
		struct utsname myname;
		if ( uname(&myname) < 0 )
		{
			perror("uname error");
			close(sendfd);
			exit(1);
		}
		bzero(&line, sizeof(line));

		snprintf(line, sizeof(line), "%s, %d\n", myname.nodename, getpid());
		for ( ; ; )
		{
			sendto( sendfd, line, strlen(line), 0,(struct sockaddr *)&addr,(socklen_t)sizeof(addr));
			{
				int n ;
				char back[256];
				struct sockaddr_in from;
				socklen_t len;
				
				len = sizeof(from);
				n = recvfrom(sendfd, back, 255, 0, (struct sockaddr *)&from, &len);
				back[n] = 0;
				printf("reply %s: %s",inet_ntoa(from.sin_addr), back);
			}
			sleep(5);
		}
	}

	return 0;
}

