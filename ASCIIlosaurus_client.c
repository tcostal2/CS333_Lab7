#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<stdbool.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string.h>
#include<poll.h>
#include<fcntl.h>
#include<ncurses.h>

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
	struct sockaddr_in src_addr;
	char* ip_addr = DEFAULT_HOST;
	world_state_t world;
	bool running = true;
	int flags;

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
				fprintf(stderr, "%d is enabled\n", is_verbose);
				break;
			case 'h':
				fprintf(stderr, "Helpful text\n");
				exit(EXIT_SUCCESS);
				break;
			default:
				fprintf(stderr, "Invalid option\n");
				exit(EXIT_FAILURE);
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

	flags = fcntl(sockfd, F_GETFL, 0);
	if(flags == -1){
		perror("F_GETFL\n");
		exit(EXIT_FAILURE);
	}
	flags |= O_NONBLOCK;
	if(fcntl(sockfd, F_SETFL, flags) == -1){
		perror("F_SETFL\n");
		exit(EXIT_FAILURE);
	}
	
	
	setup_ui();
	while(running){

		int userinpt = get_input();
		socklen_t addlen_src = sizeof(src_addr);
		if(userinpt == 'q'){
			running = false;
		}

		if(userinpt != ERR){
			sendto(sockfd, &userinpt, sizeof(int), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)); 
		}

		if(recvfrom(sockfd, &world, sizeof(world), 0, (struct sockaddr *)&src_addr, &addlen_src) != -1){

			for(int i = 0; i < MAX_PLAYERS; i++){
				if(world.players[i].active){
					world.players[i].x  = ntohl(world.players[i].x);
					world.players[i].y = ntohl(world.players[i].y);
				}
			}

			draw_world(&world);
		}
	}

	teardown_ui();
	close(sockfd);
	exit(EXIT_SUCCESS);
}
