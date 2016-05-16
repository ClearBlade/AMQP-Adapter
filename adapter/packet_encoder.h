#ifndef _packet_encoder_h
#define _packet_encoder_h

/* 
 * This constructs and sends responses back to the AMQP client
 */
void encodePacket(int packetType, int client);

#endif