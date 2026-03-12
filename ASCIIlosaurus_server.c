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
#include<signal.h>

#include "ASCIIlosaurus_world.h"

#define OPTIONS "P:vh"
#define DEFAULT_PORT 10011

volatile sig_atomic_t keep_running = true;

void sig_handler(int);

void sig_handler(int signum){
	keep_running = false;
}

int main(int argc, char* argv[]){
	int opt;
	bool is_verbose = false;	
	int sockfd; 
	int port = DEFAULT_PORT;
	struct sockaddr_in server_addr;
	struct sockaddr_in src_addr;
	world_state_t world;
	int flags;
	struct sigaction sig;

	while((opt = getopt(argc, argv, OPTIONS)) != -1){
		switch(opt){
			case 'P':
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
	sig.sa_handler = sig_handler;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0;
	sigaction(SIGINT, &sig, NULL);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

	for(int i = 0; i < MAX_PLAYERS; i++){
		world.players[i].active = false;


	exit(EXIT_SUCCESS);
}
