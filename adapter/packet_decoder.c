#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "amqp.h"
#include "helper_library.h"
#include "mqtt.h"

/*
 * This checks whether the protocol header frame is as per the spec and return 0 on successfull parsing or -1 on error
 */
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

/*
 * This decodes and parses all the Connection packets sent by the client. It pulls out the authentication and MQTT broker
 * data from the Connection.Start-OK packet and connects to the MQTT broker. If connection is successsfull it resumes the
 * AMQP interactions with the client and if not, it returns a -1 to close the connection with the client
 */
int decodeConnectionPacket(char *packet, int packetLength, int remainingLength, struct Client *connectedClient) {
	if((packet[packetLength - remainingLength] - 0 == 0x00) && (packet[packetLength - (remainingLength - 1)] - 0 == CONNECTION_START_OK)) {
		if(packet[packetLength - 1] - 0 != 0xFFFFFFFFFFFFFFCE) {
			return -1;
		}

		// I have hardcoded this. This is dangerous. The correct way is to parse the entire packet. This needs to be changed
		// This is taking 4 packets of 1 byte each and combining them to give the 4 byte length of the auth data
		uint64_t authDataLength = getLength(366, packet);

		remainingLength = packetLength - 370; // This also is hardcoded for testing

		struct ConnectionData connectionData = getConnectionData(authDataLength, packet, packetLength, remainingLength); // Get the mqtt connection details
		if(connectionData.username == NULL) {
			return -1;
		}

		char *brokerAddressTmp = getConcatString("tcp://", connectionData.mqttBrokerAddress);
		char *brokerAddressTmp2 = getConcatString(brokerAddressTmp, ":");
		char *brokerAddress = getConcatString(brokerAddressTmp2, connectionData.mqttPort); // Full MQTT broker address
		free(brokerAddressTmp);
		free(brokerAddressTmp2);
		printf("%s\n", brokerAddress);
		printf("%s\n", connectionData.username);
		printf("%s\n", connectionData.password);
		MQTTClient mqttClient = connectToMQTTBroker(brokerAddress, connectionData.clientID, connectionData.username, connectionData.password);
		if(mqttClient == NULL) {
			connectedClient->isAuthenticated = false;
			connectedClient->mqttClient = NULL;
			printf("Could not connect to ClearBlade Broker\n");

			free(brokerAddress);
			free(connectionData.clientID);
			free(connectionData.username);
			free(connectionData.password);
			free(connectionData.mqttBrokerAddress);
			free(connectionData.mqttPort);

			return -1;
		}

		free(brokerAddress);
		free(connectionData.clientID);
		free(connectionData.username);
		free(connectionData.password);
		free(connectionData.mqttBrokerAddress);
		free(connectionData.mqttPort);

		connectedClient->isAuthenticated = true;
		connectedClient->mqttClient = mqttClient;
		printf("Connected to ClearBlade Broker\n");

		return 0;
	} else if((packet[packetLength - remainingLength] - 0 == 0x00) && (packet[packetLength - (remainingLength - 1)] - 0 == CONNECTION_TUNE_OK)) { // We don't do anything here since we just want the client to think that its using AMQP
		return 0;
	} else if((packet[packetLength - remainingLength] - 0 == 0x00) && (packet[packetLength - (remainingLength - 1)] - 0 == CONNECTION_OPEN)) {
		return 0;
	}

	return -1;
}

/*
 * Checks the Channel.Open packet
 */
int decodeChannelPacket(char *packet, int packetLength, int remainingLength) {
	if((packet[packetLength - remainingLength] - 0 == 0x00) && (packet[packetLength - (remainingLength - 1)] - 0 == CHANNEL_OPEN)) {
		if(packet[packetLength - 1] - 0 != 0xFFFFFFFFFFFFFFCE) {
			return -1;
		}

		return 0;
	} else if((packet[packetLength - remainingLength] - 0 == 0x00) && (packet[packetLength - (remainingLength - 1)] - 0 == CHANNEL_CLOSE)) {
		if(packet[packetLength - 1] - 0 != 0xFFFFFFFFFFFFFFCE) {
			return -1;
		}

		return 0;
	}

	return -1;
}

/*
 * Parses the packet the get the routing-key (topic)
 */
char *getPublishTopic(char *packet, int packetLength, int *remainingLength) {
	int i = 0;
	*remainingLength = *remainingLength - 2; // We don't need the Ticket
	uint16_t topicLengthMSB = (packet[packetLength - *remainingLength] - 0) << 8;
	(*remainingLength)--;
	uint16_t topicLengthLSB = (packet[packetLength - *remainingLength] - 0) & 0x00FF;
	uint16_t topicLength = topicLengthMSB | topicLengthLSB;
	char *topic = malloc(topicLength + 1);
	if(topic == NULL)
		return NULL;
	
	(*remainingLength)--;
	while(topicLength > 0) {
		topic[i] = packet[packetLength - *remainingLength];
		(*remainingLength)--;
		topicLength--;
		i++;
	}

	topic[i] = '\0';

	return topic;
}

/*
 * Parses the packet and gets the payload
 */
char *getPublishMessage(char *packet, int packetLength, int *remainingLength) {
	int i = 0;
	uint64_t messageLength = getLength(packetLength - *remainingLength, packet);
	*remainingLength = *remainingLength - 4;

	char *message = malloc(messageLength + 1);
	if(message == NULL) {
		return NULL;
	}

	while(messageLength > 0){
		message[i] = packet[packetLength - *remainingLength];
		(*remainingLength)--;
		messageLength--;
		i++;
	}

	message[i] = '\0';

	return message;
}

/*
 * Checks the packet for the Publish method and gets the topic and message and forwards it to the MQTT broker
 */
int decodeBasicPublish(char *packet, int packetLength, int remainingLength, struct Client *client) {
	if(!client->isAuthenticated) {
		return -1;
	}

	if(client->mqttClient == NULL) {
		return -1;
	}
	
	uint64_t publishLength = getLength(3, packet);
	if(packet[publishLength + 7] - 0 != 0xFFFFFFFFFFFFFFCE)
		return -1;

	if((packet[packetLength - remainingLength] - 0 == 0x00) && (packet[packetLength - (remainingLength - 1)] - 0 == PUBLISH)) {
		remainingLength--;
		remainingLength--;
		char *topic = getPublishTopic(packet, packetLength, &remainingLength);
		if(topic == NULL)
			return -1;

		remainingLength = packetLength - (publishLength + 7);
		remainingLength--; // Go to the next frame. We have the publish topic, we don't need anything else

		if((packet[packetLength - remainingLength] - 0) != 0x02) { // Type should be content-header
			return -1;
		}

		remainingLength = remainingLength - 3;
		uint64_t contentHeaderLength = getLength(packetLength - remainingLength, packet);
		remainingLength = remainingLength - 4;

		if((packet[(packetLength - remainingLength) + contentHeaderLength] - 0) != 0xFFFFFFFFFFFFFFCE) {
			return -1;
		}

		remainingLength = packetLength - ((packetLength - remainingLength) + contentHeaderLength);
		remainingLength--; // Go tp next frame

		if(packet[packetLength - remainingLength] - 0 != 0x03) { // Type should be content-body
			return -1;
		}

		remainingLength = remainingLength - 3;
		char *message = getPublishMessage(packet, packetLength, &remainingLength);
		if(message == NULL) {
			return -1;
		}

		if(packet[packetLength - remainingLength] - 0 != 0xFFFFFFFFFFFFFFCE) {
			return -1;
		}

		publishMQTTMessage(topic, message, client->mqttClient);

	} else {
		return -1;
	}

	return 0;
}


/*
 * This decodes and parses the packet and returns the packet type and decode status back to the client handler to
 * handle the response that is to be sent to the client
 */
struct DecodedPacket decodePacket(char *packet, int packetLength, struct Client *connectedClient) {
	int remainingLength = 0; // Remaining length of the packet
	struct DecodedPacket packetDecodeStatus; // This contains the packet type and decode status

	if((packet[0] == 'A') && (packetLength == 8)) { // Protocol header packet
		remainingLength = packetLength;
		remainingLength--;

		int status = decodeProtocolHeader(packet, packetLength, remainingLength);

		if(status >= 0) {
			packetDecodeStatus.packetType = PROTOCOL_HEADER;
			packetDecodeStatus.class = CONNECTION_LSB;
			packetDecodeStatus.decodeStatus = DECODE_SUCCESS;
		} else {
			packetDecodeStatus.packetType = UNKNOWN_PACKET;
			packetDecodeStatus.decodeStatus = DECODE_UNSUCCESS;
		}
	} else if(packet[0] - 0 == METHOD) { // Parses all METHOD packets
		remainingLength = packetLength - 8;

		if((packet[packetLength - (remainingLength + 1)] - 0 == CONNECTION_MSB) && (packet[packetLength - remainingLength] - 0 == CONNECTION_LSB)) {
			remainingLength = remainingLength - 1;

			int connectionPacketStatus = decodeConnectionPacket(packet, packetLength, remainingLength, connectedClient);

			if(connectionPacketStatus >= 0) {
				packetDecodeStatus.packetType = packet[10] - 0;
				packetDecodeStatus.class = CONNECTION_LSB;
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
				packetDecodeStatus.class = CHANNEL_LSB;
				packetDecodeStatus.decodeStatus = DECODE_SUCCESS;
			} else {
				packetDecodeStatus.packetType = UNKNOWN_PACKET;
				packetDecodeStatus.decodeStatus = DECODE_UNSUCCESS;
			}
		} else if((packet[packetLength - (remainingLength + 1)] - 0 == BASIC_MSB) && (packet[packetLength - remainingLength] - 0 == BASIC_LSB)) {
			remainingLength = remainingLength - 1;

			int basicPublishStatus = decodeBasicPublish(packet, packetLength, remainingLength, connectedClient);

			if(basicPublishStatus >= 0) {
				packetDecodeStatus.packetType = packet[10] - 0;
				packetDecodeStatus.class = BASIC_LSB;
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