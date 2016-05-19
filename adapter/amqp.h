#ifndef _amqp_h
#define _amqp_h

#ifndef STDINT
#include <stdint.h>
#define STDINT
#endif

#ifndef STDBOOL
#include <stdbool.h>
#define STDBOOL
#endif

#include "MQTTClient.h"

#define METHOD 0x01

#define CONNECTION_MSB 0x00
#define CONNECTION_LSB 0x0A
#define CHANNEL_MSB 0x00
#define CHANNEL_LSB 0x14
#define BASIC_MSB 0x00
#define BASIC_LSB 0x3C

#define HEARTBEAT 0x08
#define PROTOCOL_HEADER 0x00
#define CONNECTION_START 0x0A
#define CONNECTION_START_OK 0x0B
#define CONNECTION_TUNE 0x1E
#define CONNECTION_TUNE_OK 0x1F
#define CONNECTION_OPEN 0x28
#define CONNECTION_OPEN_OK 0x29
#define CONNECTION_CLOSE 0x32
#define CONNECTION_CLOSE_OK 0x33
#define CHANNEL_OPEN 0x0A
#define CHANNEL_OPEN_OK 0x0B
#define CHANNEL_CLOSE 0x28
#define CHANNEL_CLOSE_OK 0x29
#define PUBLISH 0x28
#define CONSUME 0x14
#define CONSUME_OK 0x15
#define DELIVER 0x3C

#define FRAME_END 0xCE

#define DECODE_SUCCESS 1
#define DECODE_UNSUCCESS -2
#define UNKNOWN_PACKET -1

// Struct to store packet type, class and packet decode error code
struct DecodedPacket {
	int packetType;
	int class;
	int decodeStatus;
};

// This is used to connect to the MQTT broker. These values are parsed from the AMQP packet
struct ConnectionData {
	char *username;
	char *password;
	char *clientID;
	char *mqttBrokerAddress;
	char *mqttPort;
};

// This stores client ptoperties
struct Client {
	bool isAuthenticated;
	MQTTClient mqttClient;
	int clientSocket;
};


#endif