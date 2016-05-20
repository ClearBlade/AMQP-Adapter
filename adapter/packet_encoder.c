#include <stdio.h>
#include <string.h>
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

void sendConsumeOK(int client) {
	char buff[44];

	buff[0] = METHOD;
	buff[1] = 0x00;
	buff[2] = 0x01;
	buff[3] = 0x00;
	buff[4] = 0x00;
	buff[5] = 0x00;
	buff[6] = 0x24;
	buff[7] = BASIC_MSB;
	buff[8] = BASIC_LSB;
	buff[9] = 0x00;
	buff[10] = CONSUME_OK;
	buff[11] = 0x1F;

	char *tag = "amq.ctag-i-ROwkVXJWOeKSwlzu0YXQ";
	int i = 12;
	int n = 0;

	for(i = 12; i < 43; i++) {
		buff[i] = tag[n];
		n++;
	}

	buff[43] = FRAME_END;

	send(client, buff, 44, 0);
}

/*
 * Sends a message arrived on a subscribed topic back to the AMQP consumer
 */
void sendBasicDeliver(char *routingKey, char *payload, int client) {
	int buffLength = 55 + strlen(routingKey) + 22 + 8 + strlen(payload);
	char buff[buffLength + 1];

	buff[0] = METHOD;
	buff[1] = 0x00;
	buff[2] = 0x01;
	buff[3] = 0x00;
	buff[4] = 0x00;
	buff[5] = 0x00;
	buff[6] = 0x33;
	buff[7] = BASIC_MSB;
	buff[8] = BASIC_LSB;
	buff[9] = 0x00;
	buff[10] = DELIVER;
	buff[11] = 0x1F;

	char *tag = "amq.ctag-i-ROwkVXJWOeKSwlzu0YXQ";
	int i = 12;
	int n = 0;

	for(i = 12; i < 43; i++) {
		buff[i] = tag[n];
		n++;
	}

	buff[43] = 0x00;
	buff[44] = 0x00;
	buff[45] = 0x00;
	buff[46] = 0x00;
	buff[47] = 0x00;
	buff[48] = 0x00;
	buff[49] = 0x00;
	buff[50] = 0x01;
	buff[51] = 0x00;
	buff[52] = strlen(routingKey) >> 8;
	buff[53] = strlen(routingKey) & 0x00FF;

	i = 54;
	n = 0;

	for(i = 54; i < (strlen(routingKey) + 54); i++) {
		buff[i] = routingKey[n];
		n++;
	}

	buff[i] = FRAME_END;
	i++;
	buff[i] = 0x02; // Content-header
	i++;
	buff[i] = 0x00;
	i++;
	buff[i] = 0x01;
	i++;
	buff[i] = 0x00;
	i++;
	buff[i] = 0x00;
	i++;
	buff[i] = 0x00;
	i++;
	buff[i] = 0x0E;
	i++;
	buff[i] = BASIC_MSB;
	i++;
	buff[i] = BASIC_LSB;
	i++;
	buff[i] = 0x00;
	i++;
	buff[i] = 0x00;
	i++;
	buff[i] = strlen(payload) >> 56;
	i++;
	buff[i] = strlen(payload) >> 48;
	i++;
	buff[i] = strlen(payload) >> 40;
	i++;
	buff[i] = strlen(payload) >> 32;
	i++;
	buff[i] = strlen(payload) >> 24;
	i++;
	buff[i] = strlen(payload) >> 16;
	i++;
	buff[i] = strlen(payload) >> 8;
	i++;
	buff[i] = strlen(payload) & 0x00000000000000FF;
	i++;
	buff[i] = 0x00;
	i++;
	buff[i] = 0x00;
	i++;
	buff[i] = FRAME_END;
	i++;
	buff[i] = 0x03; // Content-body
	i++;
	buff[i] = 0x00;
	i++;
	buff[i] = 0x01;
	i++;
	buff[i] = strlen(payload) >> 24;
	i++;
	buff[i] = strlen(payload) >> 16;
	i++;
	buff[i] = strlen(payload) >> 8;
	i++;
	buff[i] = strlen(payload) & 0x000000FF;
	i++;

	int l = 0;
	for(n = i; n < (strlen(payload) + i); n++) {
		buff[n] = payload[l];
		l++;
	}

	buff[n] = FRAME_END;

	send(client, buff, buffLength + 1, 0);
}

/* 
 * This constructs and sends responses back to the AMQP client
 */
void encodePacket(int packetType, int packetClass, int client) {
	if(packetType == PROTOCOL_HEADER && packetClass == CONNECTION_LSB) {
		sendConnectionStart(client);
	} else if(packetType == CONNECTION_START_OK && packetClass == CONNECTION_LSB) {
		sendConnectionTune(client);
	} else if(packetType == CONNECTION_OPEN && packetClass == CONNECTION_LSB) {
		sendConnectionOpenOK(client);
	} else if(packetType == CHANNEL_OPEN && packetClass == CHANNEL_LSB) {
		sendChannelOpenOK(client);
	} else if(packetType == CONSUME && packetClass == BASIC_LSB) {
		sendConsumeOK(client);
	}
}