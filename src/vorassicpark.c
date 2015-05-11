#include "vorassicpark.h"

int main(int argc, char** argv)
{
    Server server;

    printf("Handling params...\n");
    handleParams(&server, argc, argv);

    printf("Starting server on port %d...\n", SERVER_PORT);
    startServer(&server);

    printf("Waiting for clients...\n");
    waitForClients(&server);

    printf("Starting game !\n");
    play(&server);

    printf("Stopping server...\n");
    stopServer(&server);
    return EXIT_SUCCESS;
}
