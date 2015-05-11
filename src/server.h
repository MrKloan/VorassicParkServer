#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

void startServer(Server*);
void stopServer(Server*);
void waitForClients(Server*);

void recvPacket(TCPsocket socket, char**);
void broadcastPacket(Server*, const char*);
void sendPacket(TCPsocket, const char*, unsigned int);
char* serializeMap(const char*);

char* strpart(const char*, const char*);

#endif // UTIL_H_INCLUDED
