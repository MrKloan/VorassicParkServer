#ifndef VORASSICPARK_H
#define VORASSICPARK_H

#define SERVER_PORT 30000
#define PACKET_MAX_SIZE 1024
#define PACKET_PLAY 1
#define PACKET_POS 2
#define PACKET_UPDATE 3
#define PACKET_SCORES 4
#define PACKET_END 5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

#include "structures.h"
#include "params.h"
#include "server.h"
#include "game.h"

#endif // VORASSICPARK_H
