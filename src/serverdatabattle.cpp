#include "serverdatabattle.h"

ServerDataBattle::ServerDataBattle() {}
ServerDataBattle::ServerDataBattle(string filename, int port) {
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

    this->port = port;
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

    for (int i=0; i<this->players.size(); i++) {  // Loop through players
        // Read the commands they sent us
        Player* player = this->players[i];
        string command = player->getNextCommand();
        if (command.size() > 0) {
            this->takeCommand(command, i);
        }
    }

    // More stuff later
}

string ServerDataBattle::takeCommand(string command, int playerIndex) {
    cout << "Command: " << command << '\n';
    if (startsWith(command, "upload")) {
        vector<string> splitCommand = splitString(command, ':');
        // 1: Owner, 2: Byte coord, 3: Program type, 4: Name
        sf::Vector2i targetCoord = readByteCoord(splitCommand[2]);
        // Do some checking on the coord, make sure there's actually an upload there
        DataBattlePiece* uploadZone = nullptr;
        int uploadIndex = -1;
        for (int i=0; i<this->pieces.size(); i++) {
            DataBattlePiece* piece = this->pieces[i];
            sf::Vector2i coord = piece->sectors[0]->coord;
            if (piece->pieceType == 'u' && piece->controller == playerIndex) {
                if ((coord.x == targetCoord.x) && (coord.y == targetCoord.y)) {
                    uploadZone = piece;
                    uploadIndex = i;
                    break;
                }
            }
        }
        if (uploadZone != nullptr) {  // If we found a valid upload zone
            string programType = splitCommand[3];
            Player* player = players[playerIndex];

            for (pair<string, int> p : player->programs) {
                if (programType == p.first) {  // If the player has that program in their inventory
                    if (player->programs[programType] > 0) {  // If they have at least 1
                        player->programs[programType]--;  // Remove 1 from their inventory
                    }
                }
            }

            // Add the program
            //Program* newProgram = new Program(PROGRAM_DB[programType]);  // Clone one from PROGRAM_DB
            Program* newProgram = new Program(programType);  // Create a new one from the definition file
            newProgram->move(targetCoord, true);
            newProgram->owner = playerIndex;
            newProgram->controller = playerIndex;
            if (splitCommand[4] == "NULL") {  // We can add more checks to piece names
                newProgram->name = "piece" + to_string(this->pieceCounter);
            }
            this->addPiece(newProgram);

            // Remove the upload zone
            this->pieces.erase(this->pieces.begin() + uploadIndex); // Remove from pieces
            delete uploadZone;  // Deallocate the memory

            // Notify everyone.  Should we use upload: or addProgram:?
            string broadcastCommand = "upload:" + to_string(playerIndex) + ":" + splitCommand[2] + ":" + splitCommand[3] + ":" + newProgram->name;
            this->broadcast(broadcastCommand);

            return "ok";
        } else {
            // Need to notify the player their upload failed
            return "upload failed";
        }

    } else if (startsWith(command, "move")) {
        // Do something, Taipu
        // 1: Piece name, 2: direction
        vector<string> splitCommand = splitString(command, ':');
        string pieceName = splitCommand[1];
        char direction = splitCommand[2][0];
        // Find our source piece
        DataBattlePiece* sourcePiece = nullptr;
        for (DataBattlePiece* piece : this->pieces) {
            if (piece->name == pieceName) {
                sourcePiece = piece;
                break;
            }
        }
        if (sourcePiece->currentMove < sourcePiece->speed) {  // If we haven't moved all we can this turn
            // See if that direction is a valid move
            sf::Vector2i coordToCheck = sf::Vector2<int>(sourcePiece->sectors[0]->coord);  // Copy the head coord
            bool valid = false;
            if (direction == 'n') {
                coordToCheck += sf::Vector2<int>(0, -1);
            } else if (direction == 's') {
                coordToCheck += sf::Vector2<int>(0, 1);
            } else if (direction == 'e') {
                coordToCheck += sf::Vector2<int>(1, 0);
            } else if (direction == 'w') {
                coordToCheck += sf::Vector2<int>(-1, 0);
            }
            // See if there's a piece occupying that coord
            DataBattlePiece* occupyingPiece = nullptr;
            for (DataBattlePiece* piece : this->pieces) {
                if ((piece->sectors[0]->coord.x == coordToCheck.x) && (piece->sectors[0]->coord.y == coordToCheck.y)) {
                    occupyingPiece = piece;
                    break;
                }
            }
            if (occupyingPiece != nullptr) {  // If something is occupying that coord
                return "no";  // Fail
                // Implement different stuff for credit pickups, flags, etc.
            }
            // If we're here, there's nothing occupying the coord we want to move on.
            if (this->grid[coordToCheck.x][coordToCheck.y]) {  // If the grid square is not empty
                sourcePiece->move(coordToCheck, false);
                // Add code for gridwalk, negawalk, gridburn
                if (sourcePiece->currentMove >= sourcePiece->speed) {
                    sourcePiece->switchToAiming();
                }
            }
            return "ok";
        }
    } else if (startsWith(command, "action")) {
        // 1: Piece name, 2: Action index, >2: Target coords
        vector<string> splitCommand = splitString(command, ':');
        string pieceName = splitCommand[1];
        int actionIndex = stoi(splitCommand[2]);
        // Do something, Taipu
        // Find our source piece
        DataBattlePiece* sourcePiece = nullptr;
        for (DataBattlePiece* piece : this->pieces) {
            if (piece->name == pieceName) {
                sourcePiece = piece;
                break;
            }
        }
        //cout << sourcePiece->actions.size() << '\n';
        ProgramAction* action = sourcePiece->actions[actionIndex];
        int maxTargets = action->numOfTargets;
        int i=0;
        // Get target coords
        vector<sf::Vector2i> targetCoords;
        while ((i < maxTargets) && (i < splitCommand.size() - 3)) {
            string targetByteCoord = splitCommand[i+3];
            sf::Vector2i targetCoord = readByteCoord(targetByteCoord);
            targetCoords.push_back(targetCoord);
            i++;
        }
        // Perform the action on those targets
        this->performAction(action, targetCoords);

        // Once we've used that action, that piece is done
        sourcePiece->noAction();
        if (sourcePiece == this->currentProgram) {  // Switch programs if necessary
            this->switchPrograms();
        }

        return "ok";

    } else if (startsWith(command, "noaction")) {
        if (startsWith(command, "noaction:")) {  // If the user specified a particular piece, hence the semicolon
            return "Not implemented";
        } else {  // No piece specified, NA the current piece
            this->currentProgram->noAction();
            this->switchPrograms();
            return "ok";
        }
    } else if (startsWith(command, "DBI")) {
        // Clients send DBI when they're ready to go.
        this->players[playerIndex]->readyup();
        // Check to see if all players are ready
        bool allReady = true;
        for (int i=0; i<this->playerCounter; i++) {  // Only check the players who will actually play, not spectators
            Player* player = this->players[i];
            if (!player->ready) {
                cout << "Player " << i << " not ready\n";
                allReady = false;
                break;
            } else {
                cout << "Player " << i << " ready\n";
            }
        }
        if (allReady) {  // Start the game
            cout << "Starting\n";
            this->broadcast("DBI:");  // Broadcast DBI to all players
            this->switchTurns();
            this->players[this->currentPlayerIndex]->sendMessage("startTurn:" + to_string(this->currentPlayerIndex));  // Notify the player their turn has started
        }
        return "ok";

    } else if (startsWith(command, "disconnect")) {
        Player* player = this->players[playerIndex];
        delete player;
        // Create a new dummy player in that slot
        Player* dummyPlayer = new Player();
        this->players[playerIndex] = dummyPlayer;
        return "ok";
    }

    return "Not implemented";
}

void ServerDataBattle::broadcast(string command) {
    // Broadcast a command to all connected players
    for (Player* player : this->players) {
        player->sendMessage(command);
    }
}
