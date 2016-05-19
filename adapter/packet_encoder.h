#ifndef _packet_encoder_h
#define _packet_encoder_h

/*
 * Sends a message arrived on a subscribed topic back to the AMQP consumer
 */
void sendBasicDeliver(char *routingKey, char *payload, int client);

/* 
 * This constructs and sends responses back to the AMQP client
 */
void encodePacket(int packetType, int packetClass, int client);

#endif