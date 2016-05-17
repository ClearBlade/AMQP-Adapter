#ifndef _packet_decoder_h
#define _packet_decoder_h

/*
 * This decodes and parses the packet and returns the packet type and decode status back to the client handler to
 * handle the response that is to be sent to the client
 */
struct DecodedPacket decodePacket(char *packet, int packetLength, struct Client *connectedClient);

#endif