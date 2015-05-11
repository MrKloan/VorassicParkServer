#include "vorassicpark.h"

void play(Server *server)
{
    short i;
    char *packet = (char*)malloc(PACKET_MAX_SIZE*sizeof(char));

    initGame(&server->game); //Init

    //On transmet la map
    broadcastPacket(server, server->game.netmap);

    //On transmet le nombre de clients et le mode de jeu
    sprintf(packet, "%hi", server->game.maxClients);
    broadcastPacket(server, packet);
    sprintf(packet, "%hi", server->game.gamemode);
    broadcastPacket(server, packet);

    //on transmet son ordre de jeu à chacun des joueurs
    for(i=0 ; i < server->game.maxClients ; i++)
    {
        sprintf(packet, "%d", i+1);
        sendPacket(server->clientsSock[i], packet, server->timeout);
    }

    //Envoi des positions de départ à chacun des clients
    for(i=0 ; i < server->game.maxClients ; i++)
    {
        sprintf(packet, "%d", PACKET_UPDATE);
        broadcastPacket(server, packet);
        sprintf(packet, "%hi", i+1);
        broadcastPacket(server, packet);
        sprintf(packet, "%hi", server->game.lastPos[i][0]);
        broadcastPacket(server, packet);
        sprintf(packet, "%hi",server->game.lastPos[i][1]);
        broadcastPacket(server, packet);
    }

    //Boucle de jeu
    while(server->game.loop)
    {
        short row, col;

        //Si le joueur peut jouer
        if(canPlay(server->game.playing, server->game.positions, server->game.lastPos, server->game.map.sizeX, server->game.map.sizeY , server->game.gamemode))
        {
            //On décrémente le compteur de tours sautés
            if(server->game.pass > 0)
                server->game.pass--;

            do
            {
                //Envoie du signal de début de tour au joueur
                sprintf(packet, "%d", PACKET_PLAY);
                sendPacket(server->clientsSock[server->game.playing-1], packet, server->timeout);

                //On récupère la ligne et la colonne pointées par le joueur
                recvPacket(server->clientsSock[server->game.playing-1], &packet);
                row = atoi(packet);
                recvPacket(server->clientsSock[server->game.playing-1], &packet);
                col = atoi(packet);
            } while(!canPlace(server->game.playing, server->game.positions, server->game.lastPos, server->game.map.sizeX, server->game.map.sizeY , server->game.gamemode, col, row));

            //Un fois la position est valide, on place le joueur sur la case et on augmente son score
            server->game.positions[row][col] = server->game.playing == 1 ? 'r' : server->game.playing == 2 ? 'j' : server->game.playing == 3 ? 'v' : server->game.playing == 4 ? 'b' : '\0';
            server->game.scores[server->game.playing-1] += server->game.map.map[row][col+1];
            server->game.lastPos[server->game.playing-1][0] = row;
            server->game.lastPos[server->game.playing-1][1] = col;

            //Envoie de la nouvelle position à tous les joueurs pour mettre à jour l'affichage
            sprintf(packet, "%d", PACKET_UPDATE);
            broadcastPacket(server, packet);
            sprintf(packet, "%hi", server->game.playing);
            broadcastPacket(server, packet);
            sprintf(packet, "%hi", row);
            broadcastPacket(server, packet);
            sprintf(packet, "%hi", col);
            broadcastPacket(server, packet);
            //Mise à jour des scores
            broadcastScores(server, &packet);
        }
        //On incrémente le compteur de tours sautés
        else
        {
            server->game.pass++;
            //Si le compteur atteint le nombre de joueurs, on quitte la boucle de jeu
            if(server->game.pass == server->game.maxClients)
                server->game.loop = 0;
        }

        //On change de joueur pour le tour suivant
        if(server->game.playing == server->game.maxClients)
            server->game.playing = 1;
        else
            server->game.playing++;
    }

    broadcastScores(server, &packet);
    sprintf(packet, "%d", PACKET_END);
    broadcastPacket(server, packet);

    free(packet);
    quitGame(&server->game); //Free
}

/**
 * Initialise les variables de jeu.
 * @param Game *game
 */
void initGame(Game *game)
{
    short i, j, k, toFind;

    game->loop = 1;
    game->pass = 0;
    game->playing = 1;

    //Allocation des tableaux
    game->scores = (short*)malloc(game->maxClients*sizeof(short));
    game->positions = (char**)malloc(game->map.sizeX*sizeof(char*));
    game->lastPos = (short**)malloc(game->maxClients*sizeof(short*));

    for(i=0 ; i < game->maxClients ; i++)
    {
        game->scores[i] = 0;
        game->lastPos[i] = (short*)malloc(2*sizeof(short)); // 2 -> row, col
    }
    for(i=0 ; i < game->map.sizeY ; i++)
    {
        game->positions[i] = (char*)malloc(game->map.sizeX*sizeof(char));

        for(j=0 ; j < game->map.sizeX ; j++)
            game->positions[i][j] = '\0';
    }

    //Placement défini des joueurs
    for(i=0, toFind = -32 ; i < game->maxClients ; i++, toFind--)
    {
        short found = 0;
        char c = i == 0 ? 'r' : i == 1 ? 'j' : i == 2 ? 'v' : i == 3 ? 'b' : '\0';

        for(j=0 ; j < game->map.sizeY ; j++)
        {
            for(k=0 ; k < game->map.sizeX ; k++)
            {
                //Si spawn défini
                if(game->map.map[j][k] == toFind)
                {
                    game->positions[j][k] = c;
                    game->lastPos[i][0] = j;
                    game->lastPos[i][1] = k-1;
                    found = 1;
                    break;
                }
            }
            if(found)
                break;
        }
        //Par défaut
        if(!found)
        {
            switch(i)
            {
                case 0:
                    game->positions[0][0] = c;
                    game->lastPos[i][0] = 0;
                    game->lastPos[i][1] = 0;
                    break;

                case 1:
                    game->positions[game->map.sizeY-1][game->map.sizeX-1] = c;
                    game->lastPos[i][0] = game->map.sizeY-1;
                    game->lastPos[i][1] = game->map.sizeX-1;
                    break;

                case 2:
                    game->positions[0][game->map.sizeX-1] = c;
                    game->lastPos[i][0] = 0;
                    game->lastPos[i][1] = game->map.sizeX-1;
                    break;

                case 3:
                    game->positions[game->map.sizeY-1][0] = c;
                    game->lastPos[i][0] = game->map.sizeY-1;
                    game->lastPos[i][1] = 0;
                    break;
            }
        }
    }
}

short canPlay(short player, char **positions, short **lastPos, short gridSizeX, short gridSizeY , short gamemode)
{
    char c = player == 1 ? 'r' : player == 2 ? 'j' : player == 3 ? 'v' : player == 4 ? 'b' : '\0';
    short i, j;

    //Conditions de jouabilité
    switch(gamemode)
    {
        //Octopus
        case 1:
            for(i=0 ; i < gridSizeY ; i++)
            {
                for(j=0 ; j < gridSizeX ; j++)
                {
                    //Pour chaque case appartenant à ce joueur...
                    if(positions[i][j] == c)
                    {
                        //... on vérifie que l'une de ses adjacentes est vide. Si oui, renvoie vrai.
                        if((i <= gridSizeY-2 && positions[i+1][j] == '\0')
                        || (i > 0 && positions[i-1][j] == '\0')
                        || (j <= gridSizeX-2 && positions[i][j+1] == '\0')
                        || (j > 0 && positions[i][j-1] == '\0')
                        || (i <= gridSizeY-2 && j <= gridSizeX-2 && positions[i+1][j+1] == '\0')
                        || (i <= gridSizeY-2 && j > 0 && positions[i+1][j-1] == '\0')
                        || (i > 0 && j > 0 && positions[i-1][j-1] == '\0')
                        || (i > 0 && j <= gridSizeX-2 && positions[i-1][j+1] == '\0'))
                            return 1;
                    }
                }
            }
            break;

        //Snake
        case 0:
            if((lastPos[player-1][0] <= gridSizeX-2 && positions[lastPos[player-1][0]+1][lastPos[player-1][1]] == '\0')
            || (lastPos[player-1][0] > 0 && positions[lastPos[player-1][0]-1][lastPos[player-1][1]] == '\0')
            || (lastPos[player-1][1] <= gridSizeY-2 && positions[lastPos[player-1][0]][lastPos[player-1][1]+1] == '\0')
            || (lastPos[player-1][1] > 0 && positions[lastPos[player-1][0]][lastPos[player-1][1]-1] == '\0'))
                return 1;
            break;
    }
    //Par défaut, on admet que le joueur ne peut pas jouer
    return 0;
}

short canPlace(short player, char **positions, short **lastPos, short gridSizeX, short gridSizeY, short gamemode, short col, short row)
{
    char c = player == 1 ? 'r' : player == 2 ? 'j' : player == 3 ? 'v' : player == 4 ? 'b' : '\0';

    printf("Player %d : %d:%d\n", player, row, col);

    if(positions[row][col] != '\0')
        return 0;

    switch(gamemode)
    {
        //Octopus
        case 1:
            if((row <= gridSizeY-2 && positions[row+1][col] == c)
            || (row > 0 && positions[row-1][col] == c)
            || (col <= gridSizeX-2 && positions[row][col+1] == c)
            || (col > 0 && positions[row][col-1] == c)
            || (row <= gridSizeY-2 && col <= gridSizeX-2 && positions[row+1][col+1] == c)
            || (row <= gridSizeY-2 && col > 0 && positions[row+1][col-1] == c)
            || (row > 0 && col > 0 && positions[row-1][col-1] == c)
            || (row > 0 && col <= gridSizeX-2 && positions[row-1][col+1] == c))
                return 1;
            break;
        //Snake
        case 0:
            if((row+1 == lastPos[player-1][0] && col == lastPos[player-1][1])
            || (row-1 == lastPos[player-1][0] && col == lastPos[player-1][1])
            || (col+1 == lastPos[player-1][1] && row == lastPos[player-1][0])
            || (col-1 == lastPos[player-1][1] && row == lastPos[player-1][0]))
                return 1;
            break;
    }
    //Par défaut, on admet que la position est invalide
    return 0;
}

void broadcastScores(Server *server, char **packet)
{
    short i;

    sprintf(*packet, "%d", PACKET_SCORES);
    broadcastPacket(server, *packet);
    for(i=0 ; i < server->game.maxClients ; i++)
    {
        sprintf(*packet, "%hi", server->game.scores[i]);
        broadcastPacket(server, *packet);
    }
}

void quitGame(Game *game)
{
    short i;

    for(i=0 ; i < game->map.sizeX ; i++)
        free(game->positions[i]);
    for(i=0 ; i < game->maxClients ; i++)
        free(game->lastPos[i]);
    free(game->positions);
    free(game->scores);
    free(game->lastPos);

    freeMap(&game->map);
}

Map loadMap(const char *name)
{
    Map map;
    int i, j;
    char str[30] = "";
    FILE *fic = NULL;

    sprintf(str, "Maps/%s", name);

    if((fic = fopen(str, "r")))
    {
        fgets(str, 30, fic);
        map.sizeX = atoi(str);

        fgets(str, 30, fic);
        map.sizeY = atoi(str);

        map.map = (int**)malloc(map.sizeY*sizeof(int*));

        fgets(str, 30, fic);
        str[strlen(str) - 1] = '\0';

        fgets(str, 30, fic);
        str[strlen(str) - 1] = '\0';

        for(i = 0 ; i < map.sizeY ; i++)
        {
            map.map[i] = (int*)malloc(map.sizeX * sizeof(int));

            for(j = 0 ; j < map.sizeX ; j++)
            {
                fscanf(fic, "%s", str);
                map.map[i][j] = atoi(str);
            }
        }
        fclose(fic);
    }
    else
    {
        fprintf(stderr, "La map %s n'est pas présente sur le serveur.", name);
        exit(EXIT_FAILURE);
    }

    return map;
}


void freeMap(Map *map)
{
    short i;

    for(i=0 ; i < map->sizeX ; i++)
        free(map->map[i]);
    free(map->map);

}
