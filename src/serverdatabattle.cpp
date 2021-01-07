#include "serverdatabattle.h"

ServerDataBattle::ServerDataBattle() {}
ServerDataBattle::ServerDataBattle(string filename) {
    this->pieceCounter = 0;
    this->filename = filename;
    this->programStartingState = nullptr;
    this->pieces.clear();
    this->currentPlayerIndex = -1;

    this->playerCounter = 0;

    for (int i=0; i<16; i++) {
        for (int j=0; j<16; j++) {
            this->grid[i][j] = 0;
        }
    }

    this->port = 9900;
    this->listener = new sf::TcpListener();
    this->listener->listen(this->port);
    this->listener->setBlocking(false);
    cout << "Listening on port " << this->port << '\n';

    this->load();
}

ServerDataBattle::~ServerDataBattle()
{
    // Do cleanup code here
}

void ServerDataBattle::tick() {
    //cout << "ServerDataBattle::tick()\n";
    // Listen for new connections
    sf::TcpSocket* client = new sf::TcpSocket();
    if (this->listener->accept(*client) == sf::Socket::Done) {  // If a new client connected
        cout << "New connection received from " << client->getRemoteAddress() << std::endl;
        // We need to send data back to the client.  Map info and such
        sf::Packet newPacket = sf::Packet();
        newPacket << "Logging into Netmap";  // We could replace this with a MotD
        client->send(newPacket);

        newPacket.clear();
        newPacket << ("map:" + this->filename);
        client->send(newPacket);

        int playerIndex = -1;  // Spectator by default
        if (this->playerCounter < this->maxPlayers) {  // If there's room for another player
            playerIndex = this->playerCounter;
            this->playerCounter++;  // Increment the counter
        }

        newPacket.clear();
        newPacket << ("players:" + to_string(this->playerCounter));
        client->send(newPacket);

        newPacket.clear();
        newPacket << ("playerIndex:" + to_string(playerIndex));
        client->send(newPacket);

        // Bring the new player up to speed, what pieces are where
        newPacket.clear();
        for (DataBattlePiece* p : this->pieces) {
            if (p->pieceType == 'u') {  // Upload
                sf::Vector2i coord = p->sectors[0]->coord;
                newPacket <<
                ("addUpload:" + to_string(coord.x) + ":"
                 + to_string(coord.y) + ":"
                 + to_string(p->owner) + "\n");

            } else if (p->pieceType == 'p') {  // Normal program
                sf::Vector2i coord = p->sectors[0]->coord;
                newPacket <<
                ("addUpload:" + p->uploadName + ":" +
                 to_string(coord.x) + ":" +
                 to_string(coord.y) + ":" +
                 to_string(p->owner) + ":" +
                 p->name + "\n");
                 // Add commands for adding sectors to the program here
            }
        }
        client->send(newPacket);

        client->setBlocking(false);
        // Create a new NetworkPlayer, give it client as its socket, and add it to the list of players
        NetworkPlayer* newPlayer = new NetworkPlayer();
        newPlayer->socket = client;
        this->players.push_back(newPlayer);

    } else {  // Connection failed for whatever reason
        //cout << "Connection failed\n";
        delete client;
    }

    // More stuff later
}
