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
	struct sockaddr_in clients[MAX_PLAYERS];
	world_state_t world;
	struct sigaction sig;
	struct pollfd fds[2];
	int user_inpt=0;
	socklen_t addrlen = sizeof(src_addr);

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
		world.players[i].x = 0;
		world.players[i].y = 0;
		world.players[i].symbol = '@';
	}

	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;
	fds[1].fd = sockfd;
	fds[1].events = POLLIN;

	while(keep_running){
		int playerindx = -1;
		int freeslot = -1;
		world_state_t updated_world;

		if(poll(fds, 2, -1) > 0){
			if(fds[0].revents & POLLIN){
				char inpt = getchar();
				if(inpt == 'q'){
					keep_running = false;
				}
			}
			if(fds[1].revents & POLLIN){
				if(recvfrom(sockfd, &user_inpt, sizeof(int), 0, (struct sockaddr*)&src_addr, &addrlen) != -1){
					for(int i = 0; i < MAX_PLAYERS; i++){
						if(world.players[i].active && clients[i].sin_port == src_addr.sin_port && clients[i].sin_addr.s_addr == src_addr.sin_addr.s_addr){
							playerindx =i;
							break;
						}
						else if (!world.players[i].active){
							if(freeslot == -1){
								freeslot = i;
							}
						}
					}
					//existing player
					if(playerindx != -1){
						if(user_inpt == 'w' || user_inpt == 'k' || user_inpt == KEY_UP){
							world.players[playerindx].y -= 1;
							if(world.players[playerindx].y < 0){
								world.players[playerindx].y = GRID_H -1;
							}
						}
						else if(user_inpt == 's' || user_inpt == 'j' || user_inpt == KEY_DOWN){
							world.players[playerindx].y += 1;
							if(world.players[playerindx].y >= GRID_H){
								world.players[playerindx].y = 0;
							}
						}
						else if(user_inpt == 'd' || user_inpt == 'l' || user_inpt == KEY_RIGHT){
							world.players[playerindx].x += 1;
							if(world.players[playerindx].x >= GRID_W){
								world.players[playerindx].x = 0;
							}
						}
						else if(user_inpt == 'a' || user_inpt == 'h' || user_inpt == KEY_LEFT){
							world.players[playerindx].x -= 1;
							if(world.players[playerindx].x < 0){
								world.players[playerindx].x = GRID_W -1;
							}
						}
						else if(user_inpt == 'q'){
							world.players[playerindx].active = false;
						}
					}
 
					//new player
					if(playerindx == -1 && freeslot != -1){
						world.players[freeslot].active = true;
						world.players[freeslot].x = GRID_W -1;
						world.players[freeslot].y = GRID_H -1;
						world.players[freeslot].symbol = 'A' + freeslot;
						clients[freeslot] = src_addr;
					}
					 
					updated_world = world;

					//update client and broadcast
					for(int i =0; i < MAX_PLAYERS; i++){
						if(updated_world.players[i].active){
							updated_world.players[i].x = htonl(updated_world.players[i].x);
							updated_world.players[i].y = htonl(updated_world.players[i].y);
						}
					}
					//send to all player clients
					for(int i =0; i < MAX_PLAYERS; i++){
						if(world.players[i].active){
							sendto(sockfd, &updated_world, sizeof(updated_world), 0, (struct sockaddr*)&clients[i], sizeof(clients[i]));
						}
					}


				}
			}
		}
	}

	exit(EXIT_SUCCESS);
}
