#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "amqp.h"
#include "packet_decoder.h"
#include "packet_encoder.h"

/*
 * This handles each client. It receives data from the client socket using select and then calls functions to decode and 
 * encode packets
 */
void *handleClientConnection(void *data) {
	int rcd; // Received bytes
	int packetLength = 0; // This is the total packet length
	int *client = (int*) data;
	int cli = *client; // The client socket descriptor
	char buff[1000]; // Buffer holding received data
	char *dataReceived = NULL;
	fd_set read_fds;
	int i;

	struct Client connectedClient;
	connectedClient.isAuthenticated = false;
	connectedClient.mqttClient = NULL;
	connectedClient.clientSocket = cli;

	FD_ZERO(&read_fds);
	FD_SET(cli, &read_fds);

	if(select(cli+1, &read_fds, NULL, NULL, NULL) == -1) {
		perror("select: ");
		exit(4);
	}

     /*
      * This loop is used to read data from the socket. It exits when there is an error and closes the client connection 
      * and exits if rcd == 0
      */
	while(1) {
		// If there is data to read on the socket, receive it!		
		if(FD_ISSET(cli, &read_fds)) {
			while((rcd = recv(cli, buff, 1000, MSG_DONTWAIT)) > 0) {
				dataReceived = malloc(rcd); // Allocate memory to fit the size of received data
				memcpy(dataReceived, buff, rcd);
				packetLength = packetLength + rcd;
				memset(buff, 0, 1000); // Reinitialize the buffer for the next iteration
			}
		}

		// Client closed the connection, exit the main loop and close the socket
		if(rcd == 0) {
			connectedClient.isAuthenticated = false;
			connectedClient.mqttClient = NULL;
			connectedClient.clientSocket = -1;
			break;
		}
		
		printf("Received Bytes: %d\n", packetLength);
		for(i = 0; i < packetLength; i ++) {
			printf("Byte %d: %d\n", i, dataReceived[i] - 0);
		}
		
		if(packetLength > 0) {
			struct DecodedPacket status = decodePacket(dataReceived, packetLength, &connectedClient); // Parse the incoming packet

			// This check whether the packet was decoded successfully and then sends a response to the client
			if(status.decodeStatus == DECODE_SUCCESS) {
				printf("Decode Success\n");
				if(status.packetType == PROTOCOL_HEADER && status.class == CONNECTION_LSB) {
					encodePacket(status.packetType, status.class, cli);
				} else if(status.packetType == CONNECTION_START_OK  && status.class == CONNECTION_LSB) {
					encodePacket(status.packetType, status.class, cli);
				} else if(status.packetType == CONNECTION_OPEN  && status.class == CONNECTION_LSB) {
					encodePacket(status.packetType, status.class, cli);
				} else if(status.packetType == CHANNEL_OPEN  && status.class == CHANNEL_LSB) {
					encodePacket(status.packetType, status.class, cli);
				} else if(status.packetType == CONSUME && status.class == BASIC_LSB) {
					encodePacket(status.packetType, status.class, cli);
				}
			} else {
				printf("Decode unsuccess\n");
				connectedClient.isAuthenticated = false;
				connectedClient.mqttClient = NULL;
				connectedClient.clientSocket = -1;
				break;
			}
		}
		
		// Reinitialize stuff for the next select() call
		FD_ZERO(&read_fds);
		FD_SET(cli, &read_fds);
		packetLength = 0;
		free(dataReceived);
		dataReceived = NULL;

		if(select(cli+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select: ");
			exit(4);
		}
	}
	

	printf("Closing connection\n");
	free(dataReceived); // Cleanup
	close(cli); 
	pthread_exit(0);
}