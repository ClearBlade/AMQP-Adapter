#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "amqp.h"
#include "helper_library.h"
#include "mqtt.h"

int decodeProtocolHeader(char *packet, int packetLength, int remainingLength) {
	if(packet[packetLength - remainingLength] != 'M')
		return -1;

	remainingLength--;

	if(packet[packetLength - remainingLength] != 'Q')
		return -1;

	remainingLength--;

	if(packet[packetLength - remainingLength] != 'P')
		return -1;

	remainingLength--;

	if(packet[packetLength - remainingLength] - 0 != 0)
		return -1;

	remainingLength--;

	if(packet[packetLength - remainingLength] - 0 != 0)
		return -1;

	remainingLength--;

	if(packet[packetLength - remainingLength] - 0 != 9)
		return -1;

	remainingLength--;

	if(packet[packetLength - remainingLength] - 0 != 1)
		return -1;

	return 0;
}

int decodeConnectionPacket(char *packet, int packetLength, int remainingLength) {
	if((packet[packetLength - remainingLength] - 0 == 0x00) && (packet[packetLength - (remainingLength - 1)] - 0 == CONNECTION_START_OK)) {
		if(packet[packetLength - 1] - 0 != 0xFFFFFFFFFFFFFFCE) {
			return -1;
		}

		uint64_t MSB = (long) packet[366] << 32;
		uint64_t MSB1 = packet[367] << 16;
		uint64_t LSB1 = packet[368] << 8;
		uint64_t LSB = packet[369] & 0x000000FF;

		uint64_t authDataLength = MSB | MSB1 | LSB1 | LSB;

		remainingLength = packetLength - 370;

		struct ConnectionData connectionData = getConnectionData(authDataLength, packet, packetLength, remainingLength);
		if(connectionData.username == NULL) {
			return -1;
		}

		char *brokerAddressTmp = getConcatString("tcp://", connectionData.mqttBrokerAddress);
		char *brokerAddressTmp2 = getConcatString(brokerAddressTmp, ":");
		char *brokerAddress = getConcatString(brokerAddressTmp2, connectionData.mqttPort);
		free(brokerAddressTmp);
		free(brokerAddressTmp2);
		printf("%s\n", brokerAddress);
		printf("%s\n", connectionData.username);
		printf("%s\n", connectionData.password);
		MQTTClient mqttClient = connectToMQTTBroker(brokerAddress, connectionData.clientID, connectionData.username, connectionData.password);
		if(mqttClient == NULL) {
			printf("Could not connect to ClearBlade Broker\n");
			return -1;
		} else {
			printf("Connected to ClearBlade Broker\n");
		}

		return 0;
	} else if((packet[packetLength - remainingLength] - 0 == 0x00) && (packet[packetLength - (remainingLength - 1)] - 0 == CONNECTION_TUNE_OK)) {
		return 0;
	} else if((packet[packetLength - remainingLength] - 0 == 0x00) && (packet[packetLength - (remainingLength - 1)] - 0 == CONNECTION_OPEN)) {
		return 0;
	}

	return -1;
}


int decodeChannelPacket(char *packet, int packetLength, int remainingLength) {
	if((packet[packetLength - remainingLength] - 0 == 0x00) && (packet[packetLength - (remainingLength - 1)] - 0 == CHANNEL_OPEN)) {
		if(packet[packetLength - 1] - 0 != 0xFFFFFFFFFFFFFFCE) {
			return -1;
		}

		return 0;
	}

	return -1;
}


struct DecodedPacket decodePacket(char *packet, int packetLength) {
	int remainingLength = 0;
	struct DecodedPacket packetDecodeStatus;

	if((packet[0] == 'A') && (packetLength == 8)) {
		remainingLength = packetLength;
		remainingLength--;

		int status = decodeProtocolHeader(packet, packetLength, remainingLength);

		if(status >= 0) {
			packetDecodeStatus.packetType = PROTOCOL_HEADER;
			packetDecodeStatus.decodeStatus = DECODE_SUCCESS;
		} else {
			packetDecodeStatus.packetType = UNKNOWN_PACKET;
			packetDecodeStatus.decodeStatus = DECODE_UNSUCCESS;
		}
	} else if(packet[0] - 0 == METHOD) {
		remainingLength = packetLength - 8;

		if((packet[packetLength - (remainingLength + 1)] - 0 == CONNECTION_MSB) && (packet[packetLength - remainingLength] - 0 == CONNECTION_LSB)) {
			remainingLength = remainingLength - 1;

			int connectionPacketStatus = decodeConnectionPacket(packet, packetLength, remainingLength);

			if(connectionPacketStatus >= 0) {
				packetDecodeStatus.packetType = packet[10] - 0;
				packetDecodeStatus.decodeStatus = DECODE_SUCCESS;
			} else {
				packetDecodeStatus.packetType = UNKNOWN_PACKET;
				packetDecodeStatus.decodeStatus = DECODE_UNSUCCESS;
			}
		} else if((packet[packetLength - (remainingLength + 1)] - 0 == CHANNEL_MSB) && (packet[packetLength - remainingLength] - 0 == CHANNEL_LSB)) {
			remainingLength = remainingLength - 1;

			int channelPacketStatus = decodeChannelPacket(packet, packetLength, remainingLength);

			if(channelPacketStatus >= 0) {
				packetDecodeStatus.packetType = packet[10] - 0;
				packetDecodeStatus.decodeStatus = DECODE_SUCCESS;
			} else {
				packetDecodeStatus.packetType = UNKNOWN_PACKET;
				packetDecodeStatus.decodeStatus = DECODE_UNSUCCESS;
			}
		}
	} else {
		packetDecodeStatus.packetType = UNKNOWN_PACKET;
		packetDecodeStatus.decodeStatus = DECODE_UNSUCCESS;
	}

	return packetDecodeStatus;
}