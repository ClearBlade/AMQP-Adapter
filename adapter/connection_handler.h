#ifndef _connection_handler_h
#define _connection_handler_h

/*
 * This handles each client. It receives data from the client socket using select and then calls functions to decode and 
 * encode packets
 */
void *handleClientConnection(void *data);

#endif