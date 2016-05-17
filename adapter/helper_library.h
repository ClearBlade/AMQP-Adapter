#ifndef _helper_library_h
#define _helper_library_h

#ifndef STDINT
#include <stdint.h>
#define STDINT
#endif


/*
 * This parses the Connection.Start-OK AMQP packet to get the connection details for the MQTT Broker. The connection
 * parameters must be in a predefined format
 */
struct ConnectionData getConnectionData(uint64_t length, char *packet, int packetLength, int remainingLength);


/*
 * This appends str2 to str1 and returns a pointer to the new string. This pointer needs to be freed by the user
 * after use to aviod memory leaks
 */
char *getConcatString(char *str1, char *str2);

/*
 * Calculates the total length from the 4 bytes in the packet
 */
uint64_t getLength(int index, char *packet);

#endif