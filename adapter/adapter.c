#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include "connection_handler.h"

/*
 * This sets up socket structs, creates a new socket, binds and starts listening. It does this in a non blocking way by
 * handling each client in a new thread
 */
void startAdapter() {
	struct addrinfo hints, *res;	// res will point to the result given by getaddrinfo()
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	int sock, cli;	// server and client socket descriptors
	int status;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;	// IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	// TCP socket
	hints.ai_flags = AI_PASSIVE;	// Fill my IP for me

	if((status = getaddrinfo(NULL, "5672", &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    	exit(1);
	}

	if((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		perror("socket: ");
		exit(-1);
	}

	printf("Creating Socket.......OK\n");

	if((bind(sock, res->ai_addr, res->ai_addrlen)) == -1) {
		perror("bind: ");
		exit(-1);
	}

	freeaddrinfo(res);	// Free the res pointer

	if((listen(sock, 8)) == -1) {
		perror("listen: ");
		exit(-1);
	}

	printf("Listening on port 5672.......OK\n");
	 /*
	  * This is the main server loop. It creates a new thread to handle each client connection
	  */
	while(1) {
		if((cli = accept(sock, (struct sockaddr *)&their_addr, &addr_size)) == -1) {
			perror("accept: ");
			exit(-1);
		}
		printf("Client connected: Identifier - %d\n", cli);

		pthread_t tid;

		pthread_attr_t attr;
		pthread_attr_init(&attr);

		pthread_create(&tid, &attr, handleClientConnection, &cli);	// handleClientConnection() in connection_handler.c handles each client
	}
}