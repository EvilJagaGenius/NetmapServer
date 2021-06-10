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

    this->load();
}

ServerDataBattle::~ServerDataBattle() {
    //cout << "~ServerDataBattle()\n";
    //delete this->listener;
}

void ServerDataBattle::tick() {
    //cout << "ServerDataBattle::tick()\n";
    for (int i=0; i<this->players.size(); i++) {  // Loop through players
        // Read the commands they sent us
        Player* player = this->players[i];
        if (player != nullptr) {
            string command = player->getNextCommand();
            if (command.size() > 0) {
                this->takeCommand(command, i);
            }
        }
    }
    // More stuff later
}

string ServerDataBattle::takeCommand(string command, int playerIndex) {
    cout << "ServerCommand: " << command << '\n';
    if (startsWith(command, "upload")) {  // Upload
        // 1: Owner, 2: Byte coord, 3: Program type, 4: Name
        vector<string> splitCommand = splitString(command, ':');
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

    } else if (startsWith(command, "buy")) {
        // 1: Piece type
        vector<string> splitCommand = splitString(command, ':');
        Program programToBuy = Program(splitCommand[1]);  // Don't need a permanent copy
        Player* player = this->players[playerIndex];
        cout << "player->credits " << to_string(player->credits) << '\n';
        cout << "programToBuy.cost " << to_string(programToBuy.cost) << '\n';
        if (player->credits >= programToBuy.cost) {
            player->credits -= programToBuy.cost;
            if (player->programs.count(splitCommand[1]) > 0) {  // If the player already has one of those
                player->programs[splitCommand[1]] += 1;  // Give them another one
            } else {  // If this is their first
                player->programs.insert({{splitCommand[1], 1}});  // Insert a new element in their program list
            }
            cout << "Telling player to buy program\n";
            player->sendMessage("buy:" + splitCommand[1]);  // Notify the player that they successfully bought the program
            return "ok";
        }
        return "failed";
    } else if (startsWith(command, "move")) {  // Move
        // 1: Piece name, 2: direction
        vector<string> splitCommand = splitString(command, ':');
        string pieceName = splitCommand[1];
        char direction = splitCommand[2][0];
        // Find our source piece
        DataBattlePiece* sourcePiece = nullptr;
        for (DataBattlePiece* piece : this->pieces) {
            if (piece->name == pieceName) {
                sourcePiece = piece;
                //cout << "Found sourcePiece\n";
                //cout << "currentMove: " << sourcePiece->currentMove << ", speed: " << sourcePiece->speed << '\n';
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
                //cout << "Found occupyingPiece, failing\n";
                return "no";  // Fail
                // Implement different stuff for credit pickups, flags, etc.
            }
            // If we're here, there's nothing occupying the coord we want to move on.
            if (this->grid[coordToCheck.x][coordToCheck.y]) {  // If the grid square is not empty
                //cout << "Valid grid sector\n";
                this->broadcast("move:" + pieceName + ":" + splitCommand[2]);  // Inform all the players
                sourcePiece->move(coordToCheck, false);
                // Add code for gridwalk, negawalk, gridburn
                if (sourcePiece->currentMove >= sourcePiece->speed) {
                    sourcePiece->switchToAiming();
                }
            }
            return "ok";
        }
    } else if (startsWith(command, "action")) {  // Action
        // 1: Piece name, 2: Action index, >2: Target coords
        vector<string> splitCommand = splitString(command, ':');
        string pieceName = splitCommand[1];
        int actionIndex = stoi(splitCommand[2]);
        // Find source piece
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
        this->broadcast(command);  // Don't bother parsing the command, just send it back to everyone
        this->performAction(action, targetCoords);

        // Once we've used that action, that piece is done
        sourcePiece->noAction();
        this->switchPrograms();

        return "ok";

    } else if (startsWith(command, "noaction")) {  // No Action
        // 1: Piece name
        vector<string> splitCommand = splitString(command, ':');
        string pieceName = splitCommand[1];
        DataBattlePiece* sourcePiece = nullptr;
        for (DataBattlePiece* piece : this->pieces) {
            if (piece->name == pieceName) {
                sourcePiece = piece;
                break;
            }
        }
        if (sourcePiece != nullptr && sourcePiece->owner == playerIndex) {
            sourcePiece->noAction();
            this->broadcast("noaction:" + pieceName);
            this->switchPrograms();
            return "ok";
        }
    } else if (startsWith(command, "DBI")) {  // Databattle Initialize
        // No arguments.  Clients send DBI when they're ready to go.
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
        }
        return "ok";

    } else if (startsWith(command, "disconnect")) {  // Disconnect
        if (playerIndex < this->maxPlayers) {
            this->playerCounter--;
        }
        this->players[playerIndex]->status = "Disconnected";
        this->players[playerIndex] = nullptr;
        return "ok";
        // Alternatively, we could set their status to "Disconnected" and let the Lobby handle deleting the player... I think this works though
        // We have multiple options here
    }

    return "Not implemented";
}

void ServerDataBattle::broadcast(string command) {
    cout << "Broadcasting: " << command << '\n';
    // Broadcast a command to all connected players
    for (Player* player : this->players) {
        player->sendMessage(command);
    }
}

void ServerDataBattle::switchPrograms() {
    // Given that each player gives commands to their pieces by name, I don't think this is important
    // Might need to rework how statuses are ticked though
    cout << "Server switching programs\n";
    cout << "Victory check: " << this->checkForVictory() << '\n';
    // See if there's another one of the player's programs ready to use
    bool foundNextPiece = false;
    for (int i=0; i<this->pieces.size(); i++) {
        DataBattlePiece* piece = this->pieces[i];
        if (piece->pieceType == 'p' && piece->controller == this->currentPlayerIndex && piece->state == 'm') {
            // If they have a valid piece ready to go... do something, Taipu
            foundNextPiece = true;
        }
    }
    if (!foundNextPiece) {  // If there wasn't a valid piece, ready to go
        // We need to switch turns.  Do something, Taipu
        this->switchTurns();
    }
}

void ServerDataBattle::switchTurns() {
    // If necessary, insert code to delete dead pieces
    cout << "Server switching turns\n";

    if (this->currentPlayerIndex == -1) {  // If we're starting from the upload phase
        // Delete all upload zones
        cout << "Deleting upload zones\n";

        bool killedAllUploads = false;
        DataBattlePiece* uploadZone = nullptr;
        int uploadIndex = -1;
        while (killedAllUploads == false) {
            killedAllUploads = true;
            uploadZone = nullptr;
            uploadIndex = -1;
            for (int i=0; i<this->pieces.size(); i++) {
                if (this->pieces[i]->pieceType == 'u') {
                    uploadZone = this->pieces[i];
                    uploadIndex = i;
                    killedAllUploads = false;
                    break;
                }
            }
            if (uploadZone != nullptr) {
                this->pieces.erase(this->pieces.begin() + uploadIndex);  // Remove from pieces
                delete uploadZone;  // Deallocate memory
            }
        }
        cout << "Upload deletion complete\n";
    }

    cout << "Searching for first program to use\n";
    bool foundFirstProgram = false;
    while (!foundFirstProgram) {  // This could go infinite
        this->currentPlayerIndex++;
        this->currentPlayerIndex = this->currentPlayerIndex % this->players.size();
        // Find a first program
        for (int i=0; i<this->pieces.size(); i++) {
            // Loop through all the pieces, prep all pieces controlled by the player, select one to be first
            DataBattlePiece* piece = this->pieces[i];
            cout << "Type: " << piece->pieceType << '\n';
            cout << "Name: " << piece->name << '\n';
            cout << "Controller: " << piece->controller << '\n';
            if (piece->controller == currentPlayerIndex) {
                piece->prepForTurn();
                if (piece->pieceType == 'p') {  // Or avatar or user, etc
                    if (!foundFirstProgram) {
                        foundFirstProgram = true;
                        this->nextProgram = piece;
                        this->nextProgramIndex = i;
                    }
                }
            }
        }
    }
    this->switchPrograms();
    this->players[this->currentPlayerIndex]->sendMessage("startTurn:" + to_string(this->currentPlayerIndex));  // Notify the player their turn has started
    // We could broadcast that as well.  Not sure.
    //cout << "Done switching turns\n";
}

void ServerDataBattle::addPlayer(NetworkPlayer* newPlayer) {
    cout << "ServerDataBattle::addPlayer()\n";
    // Get the player up to speed
    this->players.push_back(newPlayer);
    newPlayer->sendMessage("netdb:" + this->filename + ":" + to_string(this->creditLimit) + ":0:0:0");

    int playerIndex = -1;  // Spectator by default
    if (this->playerCounter < this->maxPlayers) {  // If there's room for another player
        playerIndex = this->playerCounter;
        this->playerCounter++;  // Increment the counter
    }
    // We should tweak the above block.  What if someone gets disconnected mid-game?
    newPlayer->sendMessage("players:" + to_string(this->maxPlayers));  // This is causing issues in the client, trying to render upload zones belonging to players that aren't there
    newPlayer->sendMessage("playerIndex:" + to_string(playerIndex));
    newPlayer->sendMessage("credits:" + to_string(this->creditLimit));

    // Bring the new player up to speed, what pieces are where
    for (DataBattlePiece* p : this->pieces) {
        if (p->pieceType == 'u') {  // Upload
            sf::Vector2i coord = p->sectors[0]->coord;
            newPlayer->sendMessage
            ("addUpload:" + getByteCoord(coord)
             + ":" + to_string(p->owner));

        } else if (p->pieceType == 'p') {  // Normal program
            sf::Vector2i coord = p->sectors[0]->coord;
            newPlayer->sendMessage
            ("addProgram:" + p->uploadName + ":" +
             getByteCoord(coord) + ":" +
             to_string(p->owner) + ":" +
             p->name + "\n");
             // Add commands for adding sectors to the program here
        }
    }
}
