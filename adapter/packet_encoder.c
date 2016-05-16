#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "amqp.h"


void sendConnectionStart(int client) {
	char buff[45];
	buff[0] = METHOD; // type = METHOD
	buff[1] = 0x00; // Channel 0
	buff[2] = 0x00; // Channel 0
	buff[3] = 0x00;
	buff[4] = 0x00;
	buff[5] = 0x00;
	buff[6] = 0x25;
	buff[7] = CONNECTION_MSB; // Class-ID Connection(10)
	buff[8] = CONNECTION_LSB; // Class-ID Connection(10)
	buff[9] = 0x00; // Method Start(10)
	buff[10] = CONNECTION_START; // Method Start(10)
	buff[11] = 0x00; // Versio-major
	buff[12] = 0x09; // version-minor
	buff[13] = 0x00;
	buff[14] = 0x00;
	buff[15] = 0x00;
	buff[16] = 0x00;
	buff[17] = 0x00;
	buff[18] = 0x00;
	buff[19] = 0x00;
	buff[20] = 0x0E;

	int i = 21;
	char *mechanism = "PLAIN AMQPLAIN";
	int n = 0;

	for(i = 21; i < 35; i++) {
		buff[i] = mechanism[n];
		n++;
	}

	buff[35] = 0x00;
	buff[36] = 0x00;
	buff[37] = 0x00;
	buff[38] = 0x05;

	char *locale = "en_US";
	n = 0;

	for(i = 39; i < 44; i++) {
		buff[i] = locale[n];
		n++;
	}

	buff[44] = FRAME_END;

	send(client, buff, 45, 0);
}

void sendConnectionTune(int client) {
	char buff[20];

	buff[0] = METHOD;
	buff[1] = 0x00;
	buff[2] = 0x00;
	buff[3] = 0x00;
	buff[4] = 0x00;
	buff[5] = 0x00;
	buff[6] = 0x0C;
	buff[7] = CONNECTION_MSB;
	buff[8] = CONNECTION_LSB;
	buff[9] = 0x00;
	buff[10] = CONNECTION_TUNE;
	buff[11] = 0x00; // Channel-Max
	buff[12] = 0x00; // Channel-Max
	buff[13] = 0x00; // Frame-Max
	buff[14] = 0x02; // Frame-Max
	buff[15] = 0x00; // Frame-Max
	buff[16] = 0x00; // Frame-Max
	buff[17] = 0x00; // Heartbeat 120
	buff[18] = 0x78; // Heartbeat 120
	buff[19] = FRAME_END;

	send(client, buff, 20, 0);
}

void sendConnectionOpenOK(int client) {
	char buff[13];

	buff[0] = METHOD;
	buff[1] = 0x00;
	buff[2] = 0x00;
	buff[3] = 0x00;
	buff[4] = 0x00;
	buff[5] = 0x00;
	buff[6] = 0x05;
	buff[7] = CONNECTION_MSB;
	buff[8] = CONNECTION_LSB;
	buff[9] = 0x00;
	buff[10] = CONNECTION_OPEN_OK;
	buff[11] = 0x00;
	buff[12] = FRAME_END;

	send(client, buff, 13, 0);
}

void sendChannelOpenOK(int client) {
	char buff[16];

	buff[0] = METHOD;
	buff[1] = 0x00;
	buff[2] = 0x01;
	buff[3] = 0x00;
	buff[4] = 0x00;
	buff[5] = 0x00;
	buff[6] = 0x08;
	buff[7] = CHANNEL_MSB;
	buff[8] = CHANNEL_LSB;
	buff[9] = 0x00;
	buff[10] = CHANNEL_OPEN_OK;
	buff[11] = 0x00;
	buff[12] = 0x00;
	buff[13] = 0x00;
	buff[14] = 0x00;
	buff[15] = FRAME_END;

	send(client, buff, 16, 0);
}

/* 
 * This constructs and sends responses back to the AMQP client
 */
void encodePacket(int packetType, int client) {
	if(packetType == PROTOCOL_HEADER) {
		sendConnectionStart(client);
	} else if(packetType == CONNECTION_START_OK) {
		sendConnectionTune(client);
	} else if(packetType == CONNECTION_OPEN) {
		sendConnectionOpenOK(client);
	} else if(packetType == CHANNEL_OPEN) {
		sendChannelOpenOK(client);
	}
}