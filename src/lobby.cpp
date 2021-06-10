#include "lobby.h"

Lobby::Lobby() {
    /*this->port = 9900;
    this->listener = new sf::TcpListener();
    this->listener->listen(this->port);
    this->listener->setBlocking(false);
    cout << "Listening on port " << this->port << '\n';*/
}

Lobby::Lobby(int portNum) {
    this->maxDataBattles = 4;
    for (int i=0; i<maxDataBattles; i++) {
        this->dbs.push_back(nullptr);
    }

    this->port = portNum;
    this->listener = new sf::TcpListener();
    this->listener->setBlocking(false);
    this->listener->listen(this->port);
    cout << "Listening on port " << this->port << '\n';
}

Lobby::~Lobby() {
    delete this->listener;
    for (int i=0; i<this->maxDataBattles; i++) {
        if (this->dbs[i] != nullptr) {
            this->dbs[i]->broadcast("shutdown:");
            delete this->dbs[i];
        }
    }
}

void Lobby::tick() {
    // Do cleanup work
    this->tickCleanup();
    // Tick each DataBattle
    for (int i=0; i<this->maxDataBattles; i++) {
        if (this->dbs[i] != nullptr) {
            this->dbs[i]->tick();
        }
    }
    // Listen for new connections
    sf::TcpSocket* client = new sf::TcpSocket();
    if (this->listener->accept(*client) == sf::Socket::Done) {  // If a new client connected
        cout << "New connection received from " << client->getRemoteAddress() << std::endl;
        // Create a new NetworkPlayer, give it client as its socket, and add it to the list of players
        client->setBlocking(false);
        NetworkPlayer* newPlayer = new NetworkPlayer();
        newPlayer->socket = client;
        this->players.push_back(newPlayer);
        // We need to catch the new player up to speed - let's put this in another function
        welcomePlayer(newPlayer);

    } else {  // Connection failed for whatever reason
        delete client;
    }

    // Get packets from the players
    for (int i=0; i<this->players.size(); i++) {  // Loop through players
        NetworkPlayer* player = this->players[i];
        if (player->status == "") {  // If the player's not in a databattle right now (otherwise, just leave the commands for the DB to pick up)
            // Read the commands they sent us
            string command = player->getNextCommand();
            if (command.size() > 0) {
                this->takeCommand(command, i);
            }
        }
    }
}

void Lobby::tickCleanup() {
    // What does this do?  Checks to see if any of the players have disconnected or are nullptrs, if so, deletes them.
    // For databattles, it checks if anyone is still playing, and if no one is, deletes the DB
    bool keepGoing = true;
    while (keepGoing) {
        keepGoing = false;
        // Clean up players
        for (int i=0; i<this->players.size(); i++) {
            if (this->players[i] == nullptr) {
                this->players.erase(this->players.begin()+i);
                keepGoing = true;
                break;
            } else if (startsWith(this->players[i]->status, "Disconnected")) {
                delete this->players[i];
                this->players.erase(this->players.begin()+i);
                keepGoing = true;
                break;
            }
        }
        // Clean up DBs
        for (int i=0; i<this->maxDataBattles; i++) {
            if (this->dbs[i] != nullptr) {
                if (this->dbs[i]->playerCounter == 0) {  // Kill the DB if there's no one playing
                    // We need to notify the spectators or something too
                    this->dbs[i]->broadcast("shutdown:");
                    //cout << "Deleting DB " << this->dbs[i] << '\n';
                    delete this->dbs[i];
                    this->dbs[i] = nullptr;
                }
            }
        }
    }
}

void Lobby::welcomePlayer(NetworkPlayer* player) {
    // Catching the player up to speed.  What other players are connected, what DataBattles are live, etc.
    sf::Packet packet = sf::Packet();
    string packetString = "";
    // First: what DBs are live, and their statuses?
    packetString += "dblist";
    for (int i=0; i<this->dbs.size(); i++) {
        ServerDataBattle* db = this->dbs[i];
        if (db != nullptr) {
            // ID#|players/max players|game mode|map
            packetString += (":" + to_string(i) + "|" + to_string(db->playerCounter) + "/" + to_string(db->maxPlayers) + "|gamemode|" + db->filename);
        }
    }
    if (this->dbs.size() == 0) {
        packetString += ':';
    }
    packet << packetString;
    player->socket->send(packet);
    packet.clear();
    // Second: what other players are online?
    packetString = "players";
    for (int i=0; i<this->players.size(); i++) {
        NetworkPlayer* p = this->players[i];
        if (p != nullptr) {  // Don't think this->players should contain a nullptr, but just double-checking
            // Name|color|status
            packetString += ':' + p->name + '|' + p->color + '|' + p->status;
        }
    }
    if (this->players.size() == 0) {
        packetString += ':';
    }
    packet << packetString;
    player->socket->send(packet);
}

string Lobby::takeCommand(string command, int playerIndex) {
    cout << "LobbyCommand: " << command << '\n';
    NetworkPlayer* player = this->players[playerIndex];
    if (startsWith(command, "chat")) {  // Chat
        vector<string> splitCommand = splitString(command, ':');
        string message = "";
        for (int i=1; i<splitCommand.size(); i++) {
            message += splitCommand[i];
        }
        for (int i=0; i<this->players.size(); i++) {
            if (this->players[i]->status == "") {  // Only send the message to other players loitering in the lobby
                this->players[i]->sendMessage("chat:" + player->name + ':' + message);
            }
        }
    } else if (startsWith(command, "createDB")) {  // Create DB
        vector<string> splitCommand = splitString(command, ':');  // How is this formatted?
        // First see if there's an open slot
        for (int i=0; i<this->dbs.size(); i++) {
            if (this->dbs[i] == nullptr) {
                string mapName = splitCommand[1];
                ServerDataBattle* newDB = new ServerDataBattle(mapName);  // Create the new DB
                this->dbs[i] = newDB;
                // Do something about setting the rules and such
                if (splitCommand.size() > 2) {
                    newDB->creditLimit = stoi(splitCommand[2]);
                }

                // Add the player to the new DB they've created
                newDB->addPlayer(player);
                player->status = "In DB " + i;
                return "ok";
            }
        }
    } else if (startsWith(command, "joinDB")) {  // Join DB
        vector<string> splitCommand = splitString(command, ':');
        int dbIndex = stoi(splitCommand[1]);
        if ((dbIndex < this->maxDataBattles) && (this->dbs[dbIndex] != nullptr)) {  // Error checking
            ServerDataBattle* db = this->dbs[dbIndex];
            db->addPlayer(player);
            player->status = "In DB " + dbIndex;
            return "ok";
        }

    } else if (startsWith(command, "disconnect")) {  // Disconnect
        delete player;  // Memory cleanup
        this->players[playerIndex] = nullptr;
        this->players.erase(this->players.begin()+playerIndex);  // Remove the player from the list
        // Could broadcast this to update the other players
    }
    return "ok";
}

void Lobby::broadcast(string command) {
    // Broadcast a command to all connected players
    for (Player* player : this->players) {
        player->sendMessage(command);
    }
}
