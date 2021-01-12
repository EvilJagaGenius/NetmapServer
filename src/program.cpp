#include "program.h"

Program::Program(string programType) {
    this->uploadName = programType;
    this->pieceType = 'p';
    this->currentMove = 0;
    this->size = 0;
    this->state = 'm';
    this->targetCounter = 0;
    this->owner = '0';
    this->invisibilityTimer = 0;
    this->visible = true;

    this->load();
}

Program::Program(Program* original) {  // Copy constructor
    cout << "Using copy constructor\n";
    this->pieceType = original->pieceType;
    this->name = original->name;
    this->uploadName = original->uploadName;
    this->screenName = original->screenName;
    this->spriteCoord = original->spriteCoord;

    for (ProgramSector* s : original->sectors) {
        this->sectors.push_back(new ProgramSector(s->coord));
    }

    for (int i=0; i<this->sectors.size(); i++) {
        ProgramSector* s = this->sectors[i];
        vector<sf::Vector2i> linkCoords;
        vector<int> linkIndices;
        for (ProgramSector* l : s->links) {
            linkCoords.push_back(l->coord);
            for (int j=0; j<this->sectors.size(); j++) {
                if (this->sectors[j]->coord == l->coord) {
                    linkIndices.push_back(j);
                }
            }
        }
        for (int index : linkIndices) {
            ProgramSector* sectorToLink = this->sectors[index];
            ProgramSector::linkSectors(s, sectorToLink);
        }
    }
    // Copy status effects
    for (pair<char, int> p : this->statuses) {
        this->statuses[p.first] = original->statuses[p.first];
    }

    // Copy actions
    for (ProgramAction* action : original->actions) {
        ProgramAction* newAction = new ProgramAction();
        newAction->load(action->filename);
        this->actions.push_back(newAction);
    }

    this->owner = original->owner;
    this->invisibilityTimer = original->invisibilityTimer;
    this->visible = original->visible;
    this->actions = original->actions;
    this->description = original->description;
    this->size = original->size;
    this->maxSize = original->maxSize;
    this->speed = original->speed;
    this->maxSpeed = original->speed;
    this->currentMove = original->currentMove;
    this->state = original->state;
    this->targetCounter = original->targetCounter;
}

Program::Program(DataBattlePiece* original) {  // Alt copy constructor
    cout << "Calling DataBattlePiece copy constructor\n";
    this->pieceType = original->pieceType;
    this->name = original->name;
    this->screenName = original->screenName;
    this->uploadName = original->uploadName;
    this->spriteCoord = original->spriteCoord;

    for (ProgramSector* s : original->sectors) {
        this->sectors.push_back(new ProgramSector(s->coord));
    }

    for (int i=0; i<this->sectors.size(); i++) {
        ProgramSector* s = this->sectors[i];
        cout << "Sector: " << getByteCoord(s->coord) << '\n';
        vector<sf::Vector2i> linkCoords;
        vector<int> linkIndices;
        for (ProgramSector* l : original->sectors[i]->links) {
            cout << "Adding link coord: " << getByteCoord(l->coord) << '\n';
            linkCoords.push_back(l->coord);
            for (int j=0; j<this->sectors.size(); j++) {
                if (this->sectors[j]->coord == l->coord) {
                    cout << "Adding link index: " << j << '\n';
                    linkIndices.push_back(j);
                }
            }
        }
        for (int index : linkIndices) {
            cout << i << '\n';
            cout << "Linking sectors\n";
            ProgramSector* sectorToLink = this->sectors[index];
            s->links.push_back(sectorToLink);
            s->numLinks++;
            //ProgramSector::linkSectors(s, sectorToLink);
        }
    }

    // Copy status effects
    for (pair<char, int> p : this->statuses) {
        this->statuses[p.first] = original->statuses[p.first];
    }

    // Copy actions
    for (ProgramAction* action : original->actions) {
        ProgramAction* newAction = new ProgramAction();
        newAction->load(action->filename);
        this->actions.push_back(newAction);
    }

    this->owner = original->owner;
    this->controller = original->controller;
    //this->invisibilityTimer = original->invisibilityTimer;
    //this->visible = original->visible;

    this->actions = original->actions;
    this->description = original->description;
    this->size = original->size;
    this->maxSize = original->maxSize;
    this->speed = original->speed;
    this->maxSpeed = original->speed;
    this->currentMove = original->currentMove;
    this->state = original->state;
    this->targetCounter = original->targetCounter;
}

Program::~Program() {
    // Fill this in, deallocate any sectors.
    cout << "Program deconstructor called\n";
    for (int i=0; i<this->sectors.size(); i++) {
        delete this->sectors[i];
    }
    // Also, deallocate any actions created.
    for (int i=0; i<this->actions.size(); i++) {
        delete this->actions[i];
    }
}

void Program::deleteSectors() {
    for (int i=0; i<this->sectors.size(); i++) {
        delete this->sectors[i];
    }
}

void Program::load() {
    ifstream textFile;
    //cout << "Loading Program " << this->uploadName << '\n';
    textFile.open("Data\\Programs\\" + this->uploadName + ".txt");
    string line;
    vector<string> splitLine;
    while (getline(textFile, line)) {
        splitLine = splitString(line, ':');
        if (startsWith(line, "//")) {  // Ignore comments
            continue;
        } else if (startsWith(line, "name")) {
            this->screenName = splitLine[1];
        } else if (startsWith(line, "screenName")) {
            this->screenName = splitLine[1];
        } else if (startsWith(line, "maxSize")) {
            this->maxSize = stoi(splitLine[1]);
        } else if (startsWith(line, "speed")) {
            this->speed = stoi(splitLine[1]);
        } else if (startsWith(line, "maxSpeed")) {
            this->maxSpeed = stoi(splitLine[1]);
        } else if (startsWith(line, "action")) {
            string actionName = splitLine[1];
            ProgramAction* newAction = new ProgramAction();
            newAction->load("Data\\Actions\\" + actionName + ".txt");
            this->actions.push_back(newAction);
			//this->actions.push_back(ACTION_DB[splitLine[1]]);
        } else if (startsWith(line, "sprite")) {
            this->spriteCoord = sf::Vector2i(stoi(splitLine[1]), stoi(splitLine[2]));
        } else if (startsWith(line, "description")) {
            this->description = splitLine[1];
        }
    }
}

void Program::move(sf::Vector2i coord, bool firstTime=false) {
    //cout << "Moving\n";
    if (firstTime) {
        sectors.clear();
        sectors.push_back(new ProgramSector(coord));
        this->size = 1;
    } else {
        bool doublingBack = false;
        int doubleBackIndex = -1;
        for (int i=0; i<this->sectors.size(); i++) {
            if (this->sectors[i]->coord == coord) {
                doublingBack = true;
                doubleBackIndex = i;
            }
        }

        if (doublingBack) {
            //cout << "Doubling back\n";
            // Swap the positions of the current head and the sector we're going to
            ProgramSector* doubleBackSector = this->sectors[doubleBackIndex];
            this->sectors[doubleBackIndex] = this->sectors[0];
            this->sectors[0] = doubleBackSector;
            this->currentMove++;
        } else {
            //cout << "Not doubling back\n";
            this->sectors.insert(this->sectors.begin(), new ProgramSector(coord, this->sectors[0]));  // Add new sector as the head
            //cout << "Added new head\n";
            while (this->sectors.size() > this->maxSize) {  // Trim program down if necessary
                //cout << "Trimming program down\n";
                //cout << "Size: " << this->sectors.size() << '\n';
                //cout << "Max size: " << this->maxSize << '\n';
                for (int i=0; i<this->sectors.size(); i++) {
                    ProgramSector* s = this->sectors[i];
                    if ((s->numLinks == 1) && (i != 0)) {
                        //cout << "Found singly-linked sector: (" << s->coord.x << ", " << s->coord.y << ")\n";
                        ProgramSector* connectedSector = s->links[0];
                        // Unlink the sectors
                        for (int j=0; j<connectedSector->links.size(); j++) {
                            //cout << "connectedSector: (" << connectedSector->coord.x << ", " << connectedSector->coord.y << ")\n";
                            if (connectedSector->links[j]->coord == s->coord) {
                                connectedSector->links.erase(connectedSector->links.begin() + j);
                                connectedSector->numLinks--;
                                //cout << connectedSector->numLinks << '\n';
                                break;
                            }
                        }
                        // If we have gridburn, blank out the sector
                        /*if (this->statuses['b'] != 0) {
                            level->grid[this->sectors[i]->coord.x][this->sectors[i]->coord.y] = 0;
                        }*/
                        // The DataBattle should be running checks and authentication for this
                        // Delete the sector from the program
                        delete this->sectors[i];
                        this->sectors.erase(this->sectors.begin() + i);
                        break;
                    }
                }
            }
            this->currentMove++;
        }
        this->size = this->sectors.size();
    }
}

void Program::addSector(sf::Vector2i coord, int pos=0) {
    vector<ProgramSector*>::iterator itr = sectors.begin();
    advance(itr, pos);
    ProgramSector* newSector = new ProgramSector(coord, sectors[pos]);
    sectors.push_back(newSector);
    this->size++;
}

void Program::switchToAiming() {
    cout << "Switching to aiming\n";
    this->state = 'a';
    // Should add an if statement to automatically end the turn if we don't have any actions (like the Memory Hog)
    cout << "Done switching to aiming\n";
}

void Program::noAction() {
    this->state = 'd';
    this->currentAction = nullptr;
    this->currentActionIndex = -1;
}

void Program::takeDamage(int damage) {
    cout << "Program::takeDamage() called\n";
    for (int d=0; d<damage; d++) {  // For each point of damage
        if (this->size > 1) {
            // Loop through the sectors.  Like trimming off stuff in move().
            for (int i=0; i<this->sectors.size(); i++) {
                ProgramSector* s = this->sectors[i];
                if ((s->numLinks == 1) && (i != 0)) {  // Find a singly-linked sector that isn't the head
                    ProgramSector* connectedSector = s->links[0];
                    // Unlink the sectors
                    for (int j=0; j<connectedSector->links.size(); j++) {
                        if (connectedSector->links[j]->coord == s->coord) {
                            connectedSector->links.erase(connectedSector->links.begin() + j);
                            connectedSector->numLinks--;
                            break;
                        }
                    }
                    /*// If we have gridburn, blank out the sector
                    if (this->statuses['b'] != 0) {
                        level->grid[this->sectors[i]->coord.x][this->sectors[i]->coord.y] = 0;
                    }*/
                    // Delete the sector from the program
                    delete this->sectors[i];
                    this->sectors.erase(this->sectors.begin() + i);
                    break;
                }
            }
            this->size = this->sectors.size();
        } else {
            // Kill the program
            cout << "X_X: " << this->uploadName << '\n';
            // Not sure what to do here, so I'll just set the state to 'x' (dead) and do other stuff in DataBattle::play()
            this->state = 'x';
            this->size = 0;
            break;  // If we're dead, break out of the loop (and function) early
        }
    }
}

void Program::prepForTurn() {
    cout << "Prepping " << this->name << " for turn\n";
    this->state = 'm';  // Moving
    this->currentActionIndex = -1;  // No action selected
    this->currentAction = nullptr;
    this->currentMove = 0;  // Hasn't moved yet
}
