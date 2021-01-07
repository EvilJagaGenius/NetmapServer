// Main file for Netmap dedicated server

#include <iostream>
#include <string>
#include <vector>
#include <SFML/Network.hpp>

#include "serverdatabattle.h"

using namespace std;

int main(int argc, char** argv) {

    int PORT = 9900;  // Default port is 9900
    for (int i=0; i<argc; i++) {
        cout << argv[i] << '\n';
        if (argv[i] == "-port") {
            if (argc < i + 1) {
                PORT = stoi(argv[i+1]);
            }
        }
    }
    cout << '\n';

    cout << "Starting Netmap 1.0 server\n";

    /*sf::TcpListener listener;  // Create a socket to listen to new connections
    listener.listen(PORT);
    cout << "Listening on port " << PORT << '\n';*/

    ServerDataBattle* db = new ServerDataBattle("testMPBattle");  // Create our databattle

    // We need non-blocking sockets in order to run a main loop at the same time as our server.  As long as we're using a single thread.
    // sf::TcpSocket socket;
    // socket.setBlocking(false);
    // Check for sf::Socket::Partial return statuses, and if so, resend until it works.

    while (true) {
        db->tick();
        //cout << "Main server loop\n";
        // We can do loop by loop ticks here
        // Receive packets from the clients and print them out
        // We need to move this to the Databattle class
        /*
        sf::Packet clientPacket;
        string packetString;
        for (sf::TcpSocket* s : clients) {
            int statusCode = s->receive(clientPacket);
            if (statusCode == sf::Socket::Done) {
                clientPacket >> packetString;
                cout << packetString << '\n';
            } else if (statusCode == sf::Socket::Disconnected) {
                // Do something, Taipu
                cout << "Client disconnected\n";
            }
        }*/
    }

    // We need some way to get out of this loop and do cleanup code
}
