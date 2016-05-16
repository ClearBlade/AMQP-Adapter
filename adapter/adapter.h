#ifndef _adapter_h
#define _adapter_h

/*
 * This sets up socket structs, creates a new socket, binds and starts listening. It does this in a non blocking way by
 * handling each client in a new thread
 */
void startAdapter();

#endif