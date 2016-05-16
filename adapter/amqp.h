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

#define METHOD 0x01

#define CONNECTION_MSB 0x00
#define CONNECTION_LSB 0x0A
#define CHANNEL_MSB 0x00
#define CHANNEL_LSB 0x14

#define PROTOCOL_HEADER 0x00
#define CONNECTION_START 0x0A
#define CONNECTION_START_OK 0x0B
#define CONNECTION_TUNE 0x1E
#define CONNECTION_TUNE_OK 0x1F
#define CONNECTION_OPEN 0x28
#define CONNECTION_OPEN_OK 0x29
#define CHANNEL_OPEN 0x0A
#define CHANNEL_OPEN_OK 0x0B

#define FRAME_END 0xCE

#define DECODE_SUCCESS 1
#define DECODE_UNSUCCESS -2
#define UNKNOWN_PACKET -1

// Struct to store packet type and packet decode error code
struct DecodedPacket {
	int packetType;
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

#endif