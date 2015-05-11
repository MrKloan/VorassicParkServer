#ifndef GAME_H
#define GAME_H

void play(Server*);

void initGame(Game*);
short canPlay(short, char**, short**, short, short, short);
short canPlace(short, char**, short**, short, short, short, short, short);
void broadcastScores(Server*, char**);
void quitGame(Game*);

Map loadMap(const char*);
void freeMap(Map*);

#endif // GAME_H
