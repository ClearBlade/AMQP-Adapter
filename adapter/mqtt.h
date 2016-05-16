#ifndef _mqtt_h
#define _mqtt_h

#ifndef MQTTCLIENT
#include "MQTTClient.h"
#define MQTTCLIENT
#endif

/*
 * Connects to the MQTT Broker and returns the mqtt client struct variable back
 */
MQTTClient connectToMQTTBroker(char *brokerAddress, char *clientID, char *username, char *password);

#endif