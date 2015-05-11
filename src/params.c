#include "vorassicpark.h"

void handleParams(Server *server, int argc, char** argv)
{
    if(argc != 5)
        setParams(server);
    else
    {
        Game game;

        game.maxClients = atoi(argv[1]);
        game.gamemode = atoi(argv[2]);
        game.netmap = serializeMap(argv[3]);
        game.map = loadMap(argv[3]);

        server->port = SERVER_PORT;
        server->game = game;
        server->timeout = atoi(argv[4]);
    }
}

/**
 * Définition des paramètres de jeu.
 * @param Server *server
 */
void setParams(Server* server)
{
    Game game;

    server->port = SERVER_PORT;
    game.maxClients = 2;
    game.gamemode = 0; //0 = snake, 1 = octopus
    game.netmap = serializeMap("default.map");
    game.map = loadMap("default.map");

    server->game = game;
    server->timeout = 500;
}
