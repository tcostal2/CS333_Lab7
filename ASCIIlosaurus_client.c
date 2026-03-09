#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<stdbool.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>

#include "ASCIIlosaurus_world.h"

#define OPTIONS "H:p:vh"
#define DEFAULT_PORT 10011
#define DEFAULT_HOST "131.252.208.23"

int main(int argc, char* argv[]){
	int opt;
	bool is_verbose = false;	
	int sockfd; 
	int port = DEFAULT_PORT;
	struct sockaddr_in server_addr;
	char* ip_addr = DEFAULT_HOST;

	while((opt = getopt(argc, argv, OPTIONS)) != -1){
		switch(opt){
			case 'H':
				ip_addr = optarg;
				break;
			case 'p':
				sscanf(optarg, "%d", &port);
				break;
			case 'v': 
				is_verbose = true;
				break;
			case 'h':
				fprintf(stderr, "Helpful text\n");
				exit(EXIT_SUCCESS);
				break;
			default:
				fprintf(stderr, "Invalid option\n");
				break;

		}
	}
	if(port < 1024){
		fprintf(stderr, "must provide listening port\n");
		exit(EXIT_FAILURE);
	}
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_pton(AF_INET, ip_addr, &server_addr.sin_addr);

	exit(EXIT_SUCCESS);
}
