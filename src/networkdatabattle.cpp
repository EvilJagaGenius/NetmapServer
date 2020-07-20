#include "networkdatabattle.h"

NetworkDataBattle::NetworkDataBattle() {
    // Might have to copy stuff from DataBattle()
    this->serverSocket = new sf::TcpSocket();
    this->serverSocket->setBlocking(false);
}

NetworkDataBattle::~NetworkDataBattle() {
    delete this->serverSocket;
}

void NetworkDataBattle::connect(string ipString, unsigned short port) {
    sf::IpAddress ip = sf::IpAddress(ipString);
    sf::Time timeout = sf::seconds(10.00);
    int connectionStatus = serverSocket->connect(ip, port, timeout);
    if (connectionStatus == sf::Socket::Error) {
        cout << "Error while connecting\n";
    } else {
        cout << "Connected\n";
        // Do something, Taipu
    }
}

void NetworkDataBattle::tick() {  // Override
    // Receive data from the server
    sf::Packet serverPacket;
    this->serverSocket->receive(serverPacket);
    // I would love some sort of loop like Java has with scanners.
    // while (serverSocket->hasNext()) {
    //      serverSocket->receive(serverPacket);
    //      Do something, Taipu }
    // How can we mimic that?
    // Once we've received the packet, do whatever it tells you

    // Send our data to the server
    // How do we do this?  Do we need to know which player is the local player and only send their commands?
    sf::Packet packetToSend;
    Player* localPlayer = this->players[localPlayerIndex];
    while (!(localPlayer->cmdQueue.empty())) {
        string command = localPlayer->cmdQueue.front();
        localPlayer->cmdQueue.pop();
        this->takeCommand(command, localPlayerIndex);
        packetToSend << command;
        // Is there a maximum size to a packet we might be in danger of exceeding?  Or will the socket handle that for us, splitting things into multiple packets if necessary?
    }
    this->serverSocket->send(packetToSend);
}
