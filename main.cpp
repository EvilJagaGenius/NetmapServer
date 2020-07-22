// Main file for Netmap dedicated server

#include <iostream>
#include <string>
#include <SFML/Network.hpp>

using namespace std;

int main(int argc, char** argv) {
    for (int i=0; i<argc; i++) {
        cout << argv[i];
    }
    cout << '\n';

    int PORT = 9900;
    // Do something, Taipu
    // Open a port
    sf::TcpListener listener;
    listener.listen(PORT);
    cout << "Listening on port " << PORT << '\n';

    while (true) {
        sf::TcpSocket client;
        if (listener.accept(client) == sf::Socket::Done) {
            string dummy;
            cout << "New connection received from " << client.getRemoteAddress() << std::endl;
            // We need to send data back to the client.  Map info and such
            sf::Packet newPacket = sf::Packet();
            newPacket << "Logging into Netmap";
            /*cout << "Enter anything to send a reply\n";
            cin >> dummy;*/
            client.send(newPacket);

            newPacket.clear();
            newPacket << "map:testMPBattle";
            client.send(newPacket);

            newPacket.clear();
            newPacket << "players:2";
            client.send(newPacket);

            newPacket.clear();
            newPacket << "youareplayer:0";
            client.send(newPacket);
        }
    }
}
