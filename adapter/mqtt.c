#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "MQTTClient.h"


/*
 * Connects to the MQTT Broker and returns the mqtt client struct variable back
 */
MQTTClient connectToMQTTBroker(char *brokerAddress, char *clientID, char *username, char *password) {
	MQTTClient mqttClient;
	MQTTClient_connectOptions mqttConnectOptions = MQTTClient_connectOptions_initializer;

	mqttConnectOptions.username = username;
	mqttConnectOptions.password = password;
	mqttConnectOptions.keepAliveInterval = 60;
    mqttConnectOptions.cleansession = 1;

	MQTTClient_create(&mqttClient, brokerAddress, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	int status = MQTTClient_connect(mqttClient, &mqttConnectOptions);
	if(status != MQTTCLIENT_SUCCESS) {
		return NULL;
	}

	return mqttClient;
}

/*
 * Publishes a message to the MQTT broker
 */
void publishMQTTMessage(char *topic, char *message, MQTTClient mqttClient) {
	printf("Came here\n");
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;

	pubmsg.payload = message;
	pubmsg.payloadlen = strlen(message);
	pubmsg.qos = 0;
	pubmsg.retained = 0;

	MQTTClient_publishMessage(mqttClient, topic, &pubmsg, &token);
}