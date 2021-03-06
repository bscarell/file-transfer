#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>

#include "helper.h"
#include "protocol.h"
#include "menu.h"
#include "net_commands.h"

int main (int argc, char* argv[]) {
    menu_option current_option;   // Menu action
    char        arg[256];         // Menu action argument
    char       *host;             // Hostname to connect to
    int         sock,             // Socket to host
                conn,             // Connection to the host
                result;           // Storage for the success
                                  //     status of commands

    // Get server hostname
    if (argc == 2) {
        host = argv[1];
    } else {
        fprintf(stderr, "Usage: %s host\n", argv[0]);
        exit(1);
    }

    // Initialize the connection
    printf("Connecting...");
    fflush(stdout);

    result = setup(host, &sock, &conn);
    if (result < 0) {
        fprintf(stderr, "\nCould not connect to %s:%d\n", host, SERVER_PORT);
        exit(1);
    }

    printf("\rConnected to %s:%d\n", host, SERVER_PORT);

    // Request the desired action from the user
    for (;;) {
        current_option = handle_input(host, arg, sizeof(arg));

        switch (current_option) {

            case QUIT:
                close(sock);
                exit(1);
                break;

            case LISTFILES:
                if (request_file_list(sock) < 0) fprintf(stderr, "[!!] Request File List failed.");
                break;

            case GETFILE:
                if (request_file(sock, arg) < 0) fprintf(stderr, "[!!] Request Get File failed.");
                break;

            case PUTFILE:
                if (request_put(sock, arg) < 0) fprintf(stderr, "[!!] Request Put File failed.");
                break;

            case RENAMEFILE:
                request_rename(sock, arg);
                break;

            default:
                fprintf(stderr, "[!!] Unknown command\n");
                break;
        }
    }

    return 0;
}
