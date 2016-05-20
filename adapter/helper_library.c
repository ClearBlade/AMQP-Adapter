#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "amqp.h"

/*
 * This is similar to string.split('-'). It basically splits the string by the specified token
 */
char **splitStringByToken(char* a_str, const char a_delim) {
	char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

/*
 * This parses the Connection.Start-OK AMQP packet to get the connection details for the MQTT Broker. The connection
 * parameters must be in a predefined format
 */
struct ConnectionData getConnectionData(uint64_t length, char *packet, int packetLength, int remainingLength) {
	int usernameCounter = 0; // This will hold the username length
	struct ConnectionData connectionData;
	connectionData.username = NULL;
	connectionData.password = NULL;
	connectionData.clientID = NULL;
	connectionData.mqttBrokerAddress = NULL;
	connectionData.mqttPort = NULL;

	if((packet[packetLength - remainingLength] - 0) != 0x00) { // First byte before the username is 0x00
		return connectionData;
	}

	remainingLength--;
	length--; // This is the length of the Response field in the AMQP packet

	// After the username ends, there is a byte containing 0x00. Find it and break the loop. This calculates the username
	// length
	for(int i = (packetLength - remainingLength); i < packetLength; i++) {
		length--;
		if((packet[i] - 0) == 0x00)
			break;

		usernameCounter++;
	}

	char *username = malloc(usernameCounter + 1);

	for(int j = 0; j < usernameCounter; j++) {
		username[j] = packet[packetLength - remainingLength]; // Fill up the username
		remainingLength--;
	}

	username[usernameCounter + 1] = '\0'; // Make sure to null terminate
	connectionData.username = username;
	
	if((packet[packetLength - remainingLength] - 0) != 0x00) {
		connectionData.username = NULL;
		return connectionData;
	}
	
	remainingLength--;
	char *remainingData = malloc(length + 1);

	for(int k = 0; k < length; k++) {
		remainingData[k] = packet[packetLength - remainingLength]; // Fill up the remaining data
		remainingLength--;
	}

	remainingData[length + 1] = '\0';
	
	char** tokens;
    tokens = splitStringByToken(remainingData, '-'); // Get password, clientID, mqtt broker address and port in an array

    if (tokens) {	
    	char *password = NULL;
	    char *clientID = NULL;
	    char *address = NULL;
	    char *port = NULL;

	    asprintf(&password, "%s", *(tokens + 0));
	    asprintf(&clientID, "%s", *(tokens + 1));
	    asprintf(&address, "%s", *(tokens + 2));
	    asprintf(&port, "%s", *(tokens + 3));

	    connectionData.password = password;
	    connectionData.clientID = clientID;
	    connectionData.mqttBrokerAddress = address;
	    connectionData.mqttPort = port;

	    free(*(tokens + 0));
	    free(*(tokens + 1));
	    free(*(tokens + 2));
	    free(*(tokens + 3));
        free(tokens);
    } else {
    	connectionData.username = NULL;

		return connectionData;
    }

	return connectionData;
}


/*
 * This appends str2 to str1 and returns a pointer to the new string. This pointer needs to be freed by the user
 * after use to aviod memory leaks
 */
char *getConcatString(char *str1, char *str2) {
	char *finalString = malloc(1 + strlen(str1) + strlen(str2)); // Needs to be freed by the user after use
	if(finalString == NULL)
		return NULL;

	strcpy(finalString, str1);
	strcat(finalString, str2);

	return finalString;
}

/*
 * Calculates the total length from the 4 bytes in the packet
 */
uint64_t getLength(int index, char *packet) {
	uint64_t MSB = (long) packet[index] << 32;  
	uint64_t MSB1 = packet[index + 1] << 16;
	uint64_t LSB1 = packet[index + 2] << 8;
	uint64_t LSB = packet[index + 3] & 0x000000FF;

	uint64_t total = MSB | MSB1 | LSB1 | LSB;

	return total;
}