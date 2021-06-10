// Main file for Netmap dedicated server

#include <unistd.h>  // For sleep() calls
#include <iostream>
#include <string>
#include <vector>
#include <SFML/Network.hpp>

#include "serverdatabattle.h"  // Do we need this anymore?
#include "lobby.h"

using namespace std;

Lobby* LOBBY;

void cleanup() {  // Called when the server closes
    cout << "Cleaning up...\n";
    delete LOBBY;
    //sleep(1);
}

int main(int argc, char** argv) {
    atexit(cleanup);

    int PORT = 9900;  // Default port is 9900
    for (int i=0; i<argc; i++) {  // Read command-line arguments
        cout << argv[i] << '\n';
        if (argv[i] == "-port") {
            if (argc < i + 1) {
                PORT = stoi(argv[i+1]);
            }
        }
    }
    cout << '\n';

    cout << "Starting Netmap 1.0 server\n";

    LOBBY = new Lobby(PORT);
    while (true) {
        LOBBY->tick();
    }
}
