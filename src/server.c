#include "vorassicpark.h"

/**
 * Démarrage de l'interface d'écoute du serveur.
 * @param Server *server
 */
void startServer(Server *server)
{
    //Initialisation SDL_net
    if(SDLNet_Init() < 0)
	{
		fprintf(stderr, "%s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	//Initialise l'interface d'écoute sur le port server->port
	if(SDLNet_ResolveHost(&server->ip, NULL, server->port) < 0)
	{
		fprintf(stderr, "%s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
    //Initialise la socket serveur et commence l'écoute
	if(!(server->socket = SDLNet_TCP_Open(&server->ip)))
	{
		fprintf(stderr, "%s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

    //Allocation des tableaux
	server->clientsSock = (TCPsocket*)malloc(server->game.maxClients*sizeof(TCPsocket));
    server->clientsIP = (IPaddress**)malloc(server->game.maxClients*sizeof(IPaddress*));

    server->connected = 0;
}

/**
 * Fermeture des sockets et libération de la mémoire.
 * @param Server *server
 */
void stopServer(Server *server)
{
    short i;

    if(server->connected > 0)
    {
        for(i=0 ; i < server->connected ; i++)
            SDLNet_TCP_Close(server->clientsSock[i]);
    }

    free(server->clientsSock);
    free(server->clientsIP);
    SDLNet_TCP_Close(server->socket);

    //Fermeture de SDLNet
    SDLNet_Quit();
}

/**
 * Attend la connexion de tous les clients avant de rendre la main au programme.
 * @param Server *server
 */
void waitForClients(Server *server)
{
    short i, loop;

    for(i=0 ; i < server->game.maxClients ; i++)
    {
        loop = 1;
        while(loop)
        {
            if((server->clientsSock[i] = SDLNet_TCP_Accept(server->socket)))
            {
                server->clientsIP[i] = SDLNet_TCP_GetPeerAddress(server->clientsSock[i]);
                server->connected++;
                loop--;
            }
        }
    }
}

/**
 * Interrompt le programme jusqu'à la réception d'un paquet à travers la socket passée en paramètre.
 * @param TCPsocket socket
 * @param char **packet
 */
void recvPacket(TCPsocket socket, char **packet)
{
    while(1)
        if(SDLNet_TCP_Recv(socket, *packet, PACKET_MAX_SIZE) > 0)
            break;
}

/**
 * Envoie un paquet à l'ensemble des clients connectés au serveur.
 * @param Server *server
 * @param char *packet
 */
void broadcastPacket(Server *server, const char *packet)
{
    short i;

    for(i=0 ; i < server->connected ; i++)
    {
        sendPacket(server->clientsSock[i], packet, server->timeout);
        SDL_Delay(server->timeout);
    }
}

/**
 * Envoie un paquet à travers la socket passée en paramètre.
 * @param TCPsocket socket
 * @param char *packet
 */
void sendPacket(TCPsocket socket, const char *packet, unsigned int timeout)
{
    //Si vrai, la connexion avec le client est perdue. Sinon, le message a bien été envoyé.
    if(SDLNet_TCP_Send(socket, (void*)packet, strlen(packet)+1) < strlen(packet)+1)
    {
        fprintf(stderr, "%s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_Delay(timeout);
}

/**
 * Renvoie une version serialisée de la map de jeu.
 * @param const char *name
 * @return char*
 */
char* serializeMap(const char *name)
{
    char* serialized = (char*)malloc(PACKET_MAX_SIZE*sizeof(char));
    FILE *file;
    char path[50] = "Maps/";
    strcat(path, name);

    //Si le fichier existe
    if((file = fopen(path, "r")))
    {
        char buffer[128];

        while(fgets(buffer, 128, file) != NULL)
            strcat(serialized, buffer);

        fclose(file);
        return serialized;
    }
    else
    {
        fprintf(stderr, "La map %s n'est pas présente sur le serveur.", path);
        exit(EXIT_FAILURE);
    }

    return serialized;
}

/**
 * Renvoie la sous-chaîne présente avant la première occurence de subject.
 * @param const char *str
 * @param const char *subject
 * @return char*
 */
char* strpart(const char *str, const char *subject)
{
    char *result = strstr(str, subject);

    if(result)
    {
        char *ret = (char*)malloc(4);
        strncpy(ret, str, strlen(str) - strlen(result));
        return ret;
    }
    else
        return NULL;
}
