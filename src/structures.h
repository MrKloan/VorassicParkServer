#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct {
    int **map;
    short sizeX;
    short sizeY;
    char* background;
    char* music;
} Map;

typedef struct {
    //Configs
    short maxClients; //1-4
    short gamemode; //0 = octopus, 1 = snake
    char* netmap; //Serialized map

    //Game values
    short loop;
    short pass;
    short playing;
    short *scores;
    char **positions;
    short **lastPos;
    Map map;
} Game;

typedef struct {
    Game game;

    unsigned short port;
    TCPsocket socket;
    IPaddress ip;
    unsigned int timeout;

    unsigned short connected;
    TCPsocket *clientsSock;
    IPaddress **clientsIP;
} Server;

#endif // STRUCTURES_H
