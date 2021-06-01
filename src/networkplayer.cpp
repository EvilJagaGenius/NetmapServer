#include "networkplayer.h"

NetworkPlayer::NetworkPlayer() {
    this->ready = false;
    this->status = "";
}

NetworkPlayer::~NetworkPlayer() {
    delete this->socket;
}

string NetworkPlayer::getNextCommand() {  // Override
    //cout << "NetworkPlayer::getNextCommand()\n";
    // See if there's anything waiting on the socket
    sf::Packet clientPacket;
    string packetString;
    int statusCode;
    statusCode = this->socket->receive(clientPacket);
    if (statusCode == sf::Socket::Done) {
        clientPacket >> packetString;
        if (packetString.size() > 0) {
            cout << "CLIENT: " << packetString << '\n';
            this->cmdQueue.emplace(packetString); // Add the string to the command queue
        }
    } else if (statusCode == sf::Socket::Disconnected) {
        // Client disconnected.  We can return a command to remove this player
        cout << "Client disconnected\n";
        return "disconnect:";
    }

    if (!(cmdQueue.empty())) {
        string returnString = this->cmdQueue.front();
        this->cmdQueue.pop();
        return returnString;
    }
    return "";
}

void NetworkPlayer::sendMessage(string message) {  // Override
    cout << "Sending message: " << message << '\n';
    sf::Packet packetToSend;
    packetToSend << message;
    int statusCode = sf::Socket::Partial;
    while (statusCode != sf::Socket::Done) {  // Since client sockets aren't blocking, retry until this goes through
        statusCode = this->socket->send(packetToSend);
    }
}
