#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "amqp.h"
#include "MQTTClient.h"
#include "packet_encoder.h"

struct MQTTSub {
	MQTTClient mqttClient;
	char *topic;
};

bool msgArrived = false;
int clientSocket;


int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
	msgArrived = true;

	sendBasicDeliver(topicName, (char *) message->payload, int clientSocket);

	return 1;
}

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
	MQTTClient_setCallbacks(mqttClient, NULL, NULL, messageArrived, NULL);
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
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;

	pubmsg.payload = message;
	pubmsg.payloadlen = strlen(message);
	pubmsg.qos = 0;
	pubmsg.retained = 0;

	MQTTClient_publishMessage(mqttClient, topic, &pubmsg, &token);
}


void *subscribe(void *data) {
	struct MQTTSub *sub = (struct MQTTSub*) data;
	MQTTClient_subscribe(sub->mqttClient, sub->topic, 0);

	while(!msgArrived) {

	}

	free(sub->topic);
	pthread_exit(0);
}

/*
 * Subscribes to a topic on the MQTT broker
 */
void subscribeToMQTTTopic(char *topic, struct Client *client) {
	struct MQTTSub sub;
	sub.mqttClient = client->mqttClient;
	sub.topic = topic;

	clientSocket = client->clientSocket;

	pthread_t tid;

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&tid, &attr, subscribe, &sub);
}