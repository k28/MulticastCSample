#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int main(int argc, char **argv)
{
	int sockfd;
	const int on = 1;
	socklen_t salen;
	struct sockaddr_in bindAddr, addr;
	struct ip_mreq group;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if ( sockfd < 0 )
	{
		perror("opening datagram socket");
		exit(1);
	}

	if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0 )
	{
		perror("Setting SO_REUSEADDR");
		close(sockfd);
		exit(1);
	}

	//Bind to the proper port number.
	bzero( &bindAddr, sizeof(bindAddr));
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(12345);
	bindAddr.sin_addr.s_addr = INADDR_ANY;

	if ( bind(sockfd, (struct sockaddr *)&bindAddr, sizeof(bindAddr)) )
	{
		perror("binding datagram socket");
		close(sockfd);
		exit(1);
	}

	/* Join the multicast group 225.1.1.1 */
	group.imr_multiaddr.s_addr = inet_addr("239.255.1.2");
	//group.imr_interface.s_addr = inet_addr("127.0.0.1");	// local interface is selected by the curnel.
	group.imr_interface.s_addr = INADDR_ANY;	// local interface is selected by the curnel.
	if ( setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0 )
	{
		perror("adding multicast group");
		close(sockfd);
		exit(1);
	}

	// Read From Stream;
	{
		int n;
		char line[256];
		socklen_t len;
		struct sockaddr_in safrom;
		for ( ; ; )
		{
			len = sizeof(safrom);
			n = recvfrom(sockfd, line, 255, 0, (struct sockaddr *)&safrom, &len);
			line[n] = 0; // NULL
			printf("from %s: %s\n",inet_ntoa(safrom.sin_addr), line);

			sendto(sockfd, "Hello!\n", 8, 0, (struct sockaddr *)&safrom, len);
		}	
	}

	return 0;
}

