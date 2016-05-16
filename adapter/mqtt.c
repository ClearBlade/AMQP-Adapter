#include <stdio.h>
#include <stdbool.h>
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