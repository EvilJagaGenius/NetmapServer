#include "programaction.h"

// ProgramAction
ProgramAction::ProgramAction() {
    this->numOfTargets = 1;  // 1 target by default
}
ProgramAction::~ProgramAction() {}
void ProgramAction::load(string filename) {
    // Do something, Taipu
    this->filename = filename;
    cout << "Loading action " << filename << '\n';
    ifstream textFile;
    textFile.open(filename);
    string line;
    char loading = '0';

    while (getline(textFile, line)) {
        if (startsWith(line, "//")) {  // Ignore comments
            continue;
        } else if (startsWith(line, "name:")) {
            this->actionName = splitString(line, ':')[1];
        } else if (startsWith(line, "desc:")) {
            this->description = splitString(line, ':')[1];
        } else if (startsWith(line, "targets:")) {
            this->numOfTargets = stoi(splitString(line, ':')[1]);
        } else if (startsWith(line, "range:")) {
            this->range = stoi(splitString(line, ':')[1]);
        } else if (startsWith(line, "sprite:")) {
            this->targetSprite = stoi(splitString(line, ':')[1]);
        } else if (startsWith(line, "COMMANDS")) {
            loading = 'c';
            continue;
        } else if (startsWith(line, "PREREQS")) {
            loading = 'p';
            continue;
        }
        if (loading == 'c') {  // Commands
            if (startsWith(line, "END_COMMANDS")) {
                loading = '0';
                continue;
            } else {
                this->commands.push_back(line);
            }
        }
        if (loading == 'p') {  // Prereqs
            if (startsWith(line, "END_PREREQS")) {
                loading = '0';
                continue;
            } else {
                this->prereqs.push_back(line);
            }
        }
    }
    textFile.close();
}

vector<sf::Vector2i> ProgramAction::getAimArea(sf::Vector2i origin, int targetNum) {
    return getRadius(this->range, origin, false);
}
bool ProgramAction::checkPrereqs(DataBattlePiece* p) {
    return true;
}

/*
// Slice (use as template)
Slice::Slice() {
    this->actionName = "Slice";
    this->description = "Slice \n Delete 2 sectors from target";
    this->range = 1;
    this->targetSprite = 0;
}
Slice::~Slice() {}
void Slice::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    //cout << "Using Slice\n";
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    DataBattlePiece* target = nullptr;

    // Search defenders
    if ((abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) && (targetCoord != origin)) {  // If in range
        for (pair<string, DataBattlePiece*> p : db->defenders) {
            for (ProgramSector* s : p.second->sectors) {
                if (s->coord == targetCoord) {
                    // We've found our target
                    target = p.second;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }

        // Search friendlies (delete if you want to disable friendly fire)
        if (target == nullptr) {
            for (DataBattlePiece* p : db->friendlies) {
                for (ProgramSector* s : p->sectors) {
                    if (s->coord == targetCoord) {
                        target = p;
                        break;
                    }
                }
                if (target != nullptr) {
                    break;
                }
            }
        }

        // Take damage
        if (target != nullptr) {
            target->takeDamage(db, 2);
        }
    }
}

Slash::Slash() {
    this->actionName = "Slash";
    this->description = "Slash \n Size required: max \n Delete 3 sectors from target";
    this->range = 1;
    this->targetSprite = 0;
}
Slash::~Slash() {}
void Slash::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    //cout << "Using Slash\n";
    if (checkPrereqs(source)) {
        sf::Vector2i origin = source->sectors[0]->coord;
        sf::Vector2i targetCoord = targets[0];
        DataBattlePiece* target = nullptr;

        // Search defenders
        if ((abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) && (targetCoord != origin)) {  // If in range
            for (pair<string, DataBattlePiece*> p : db->defenders) {
                for (ProgramSector* s : p.second->sectors) {
                    if (s->coord == targetCoord) {
                        // We've found our target
                        target = p.second;
                        break;
                    }
                }
                if (target != nullptr) {
                    break;
                }
            }

            // Search friendlies (delete if you want to disable friendly fire)
            if (target == nullptr) {
                for (DataBattlePiece* p : db->friendlies) {
                    for (ProgramSector* s : p->sectors) {
                        if (s->coord == targetCoord) {
                            target = p;
                            break;
                        }
                    }
                    if (target != nullptr) {
                        break;
                    }
                }
            }

            // Take damage
            if (target != nullptr) {
            target->takeDamage(db, 3);
            }
        }
    }
}
bool Slash::checkPrereqs(DataBattlePiece* p) {
    if (p->size >= p->maxSize) {
        return true;
    }
    return false;
}

// Stone
Stone::Stone() {
    this->actionName = "Stone";
    this->description = "Stone \n Range: 3 \n Delete 1 sector from target";
    this->range = 3;
    this->targetSprite = 0;
}
Stone::~Stone() {}
void Stone::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    //cout << "Using Stone\n";
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    DataBattlePiece* target = nullptr;

    // Search defenders
    if ((abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) && (targetCoord != origin)) {  // If in range
        for (pair<string, DataBattlePiece*> p : db->defenders) {
            for (ProgramSector* s : p.second->sectors) {
                if (s->coord == targetCoord) {
                    // We've found our target
                    target = p.second;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }
        // Search friendlies (delete if you want to disable friendly fire)
        if (target == nullptr) {
            for (DataBattlePiece* p : db->friendlies) {
                for (ProgramSector* s : p->sectors) {
                    if (s->coord == targetCoord) {
                        target = p;
                        break;
                    }
                }
                if (target != nullptr) {
                    break;
                }
            }
        }

        // Take damage
        if (target != nullptr) {
            target->takeDamage(db, 1);
        }
    }
}

Sling1::Sling1() {
    this->actionName = "Sling1";
    this->description = "Sling1 \n Range: 2 \n Warp program to selected grid sector";
    this->range = 2;
    this->targetSprite = 1;
}
Sling1::~Sling1() {}
void Sling1::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    //cout << "Using Sling1\n";
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range && (targetCoord != origin)) {  // If in range
        // Calculate the vector we're going to warp along.
        sf::Vector2i warpVector = sf::Vector2<int>(targetCoord.x - origin.x, targetCoord.y - origin.y);
        vector<sf::Vector2i> coordsToAmputate;
        // Shift every sector in the program by that vector.
        for (ProgramSector* s : source->sectors) {
            sf::Vector2i newCoord = sf::Vector2<int>(s->coord.x + warpVector.x, s->coord.y + warpVector.y);
            if (!startsWith(db->lookAt(newCoord), "tile")) {  // If that coord didn't land on a tile
                coordsToAmputate.push_back(newCoord);  // Mark it for amputation
            }
            s->coord = newCoord;
        }
        // Loop through all the sectors we marked and amputate them
        for (sf::Vector2i coord : coordsToAmputate) {
            source->amputate(db, coord);
        }
    }
}

Grow::Grow() {
    this->actionName = "Grow";
    this->description = "Grow \n Add 2 sectors to target";
    this->range = 1;
    this->targetSprite = 2;
}
Grow::~Grow() {}
void Grow::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    //cout << "Using Grow\n";
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    DataBattlePiece* target = nullptr;

    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range && (targetCoord != origin)) {  // If in range
        for (pair<string, DataBattlePiece*> p : db->defenders) {
            for (ProgramSector* s : p.second->sectors) {
                if (s->coord == targetCoord) {
                    // We've found our target
                    target = p.second;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }
        // Search friendlies (delete if you want to disable friendly fire)
        if (target == nullptr) {
            for (DataBattlePiece* p : db->friendlies) {
                for (ProgramSector* s : p->sectors) {
                    if (s->coord == targetCoord) {
                        target = p;
                        break;
                    }
                }
                if (target != nullptr) {
                    break;
                }
            }
        }

        // Take damage
        if (target != nullptr) {
            target->grow(db, 2);
        }
    }
}

Surgery1::Surgery1() {
    this->actionName = "Surgery1";
    this->description = "Surgery1 \n Size req: max \n Add 1 to target's maximum size";
    this->range = 1;
    this->targetSprite = 2;
}
Surgery1::~Surgery1() {}
void Surgery1::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    //cout << "Using Surgery1\n";
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    DataBattlePiece* target = nullptr;

    if (this->checkPrereqs(source)) {
        if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range && (targetCoord != origin)) {  // If in range
            for (pair<string, DataBattlePiece*> p : db->defenders) {
                for (ProgramSector* s : p.second->sectors) {
                    if (s->coord == targetCoord) {
                        // We've found our target
                        target = p.second;
                        break;
                    }
                }
                if (target != nullptr) {
                    break;
                }
            }
            // Search friendlies (delete if you want to disable friendly fire)
            if (target == nullptr) {
                for (DataBattlePiece* p : db->friendlies) {
                    for (ProgramSector* s : p->sectors) {
                        if (s->coord == targetCoord) {
                            target = p;
                            break;
                        }
                    }
                    if (target != nullptr) {
                        break;
                    }
                }
            }

            // Take damage
            if (target != nullptr) {
                // You should replace this with a general-purpose function that we can override for non-programs.  Say, users and avatars
                target->maxSize++;
            }
        }
    }
}
bool Surgery1::checkPrereqs(DataBattlePiece* p) {
    if (p->size >= p->maxSize) {
        return true;
    }
    return false;
}

Glitch::Glitch() {
    this->actionName = "Glitch";
    this->description = "Glitch \n Delete 2 sectors from target";
    this->range = 1;
    this->targetSprite = 0;
}
Glitch::~Glitch() {}
void Glitch::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    sf::Vector2i origin = source->sectors[0]->coord;
    sf::Vector2i targetCoord = targets[0];
    DataBattlePiece* target = nullptr;

    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range && (targetCoord != origin)) {  // If in range
        for (pair<string, DataBattlePiece*> p : db->defenders) {
            for (ProgramSector* s : p.second->sectors) {
                if (s->coord == targetCoord) {
                    // We've found our target
                    target = p.second;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }
        // Search friendlies (delete if you want to disable friendly fire)
        if (target == nullptr) {
            for (DataBattlePiece* p : db->friendlies) {
                for (ProgramSector* s : p->sectors) {
                    if (s->coord == targetCoord) {
                        target = p;
                        break;
                    }
                }
                if (target != nullptr) {
                    break;
                }
            }
        }

        // Take damage
        if (target != nullptr) {
            target->takeDamage(db, 2);
        }
    }
}

// Swarm1
Swarm1::Swarm1() {
    this->actionName = "Swarm1";
    this->description = "Swarm1 \n Range: 2, move req: 0 \n Delete 1 sector from 2 targets";
    this->numOfTargets = 2;
    this->range = 2;
    this->targetSprite = 0;
}
Swarm1::~Swarm1() {}
void Swarm1::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    if (this->checkPrereqs(source)) {
        // Do something, Taipu
        sf::Vector2i origin = source->sectors[0]->coord;
        for (int i=0; i<targets.size(); i++) {
            sf::Vector2i targetCoord = targets[i];
            DataBattlePiece* target = nullptr;

            if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range && (targetCoord != origin)) {  // If in range
                for (pair<string, DataBattlePiece*> p : db->defenders) {
                    for (ProgramSector* s : p.second->sectors) {
                        if (s->coord == targetCoord) {
                            // We've found our target
                            target = p.second;
                            break;
                        }
                    }
                    if (target != nullptr) {
                        break;
                    }
                }
                // Search friendlies (delete if you want to disable friendly fire)
                if (target == nullptr) {
                    for (DataBattlePiece* p : db->friendlies) {
                        for (ProgramSector* s : p->sectors) {
                            if (s->coord == targetCoord) {
                                target = p;
                                break;
                            }
                        }
                        if (target != nullptr) {
                            break;
                        }
                    }
                }

                // Take damage
                if (target != nullptr) {
                    target->takeDamage(db, 1);
                }
            }
        }
    }
}
bool Swarm1::checkPrereqs(DataBattlePiece* p) {
    if (p->currentMove == 0) {
        return true;
    }
    return false;
}

// Zero
Zero::Zero() {
    this->actionName = "Zero";
    this->description = "Zero \n Delete an open grid sector";
    this->range = 1;
    this->targetSprite = 1;
}
Zero::~Zero() {}
void Zero::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) {  // If in range
        if (startsWith(db->lookAt(targetCoord), "tile")) { // If it's a tile
            db->flipSector(targetCoord);
        }
    }
}

// One
One::One() {
    this->actionName = "One";
    this->description = "One \n Create an open grid sector";
    this->range = 1;
    this->targetSprite = 1;
}
One::~One() {}
void One::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) {  // If in range
        if (startsWith(db->lookAt(targetCoord), "empty")) { // If it's empty space
            db->flipSector(targetCoord);
        }
    }
}


// Cut
Cut::Cut() {
    this->actionName = "Cut";
    this->description = "Cut \n Delete 2 sectors from target";
    this->range = 1;
    this->targetSprite = 0;
}
Cut::~Cut() {}
void Cut::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    //cout << "Using Cut\n";
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    DataBattlePiece* target;

    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) {  // If in range
        // Search friendlies
        for (DataBattlePiece* p : db->friendlies) {
            for (ProgramSector* s : p->sectors) {
                if (s->coord == targetCoord) {
                    target = p;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }

        // Take damage
        if (target != nullptr) {
            target->takeDamage(db, 2);
        }
    }
}

// SpotLight

// Byte
Byte::Byte() {
    this->actionName = "Byte";
    this->description = "Byte \n Delete 2 sectors from target";
    this->range = 1;
    this->targetSprite = 0;
}
Byte::~Byte() {}
void Byte::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    DataBattlePiece* target;

    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) {  // If in range
        // Search friendlies
        for (DataBattlePiece* p : db->friendlies) {
            for (ProgramSector* s : p->sectors) {
                if (s->coord == targetCoord) {
                    target = p;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }

        // Take damage
        if (target != nullptr) {
            target->takeDamage(db, 2);
        }
    }
}

// PackRun

// Phaser
Phaser::Phaser() {
    this->actionName = "Phaser";
    this->description = "Phaser \n Range: 2 \n Delete 2 sectors from target";
    this->range = 2;
    this->targetSprite = 0;
}
Phaser::~Phaser() {}
void Phaser::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    DataBattlePiece* target;

    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) {  // If in range
        // Search friendlies
        for (DataBattlePiece* p : db->friendlies) {
            for (ProgramSector* s : p->sectors) {
                if (s->coord == targetCoord) {
                    target = p;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }

        // Take damage
        if (target != nullptr) {
            target->takeDamage(db, 2);
        }
    }
}

// BlindEye
BlindEye::BlindEye() {
    this->actionName = "BlindEye";
    this->description = "BlindEye \n Range: 2 \n Cloak a target for 1 turn";
    this->range = 2;
    this->targetSprite = 2;
}
BlindEye::~BlindEye() {}
void BlindEye::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    DataBattlePiece* target = nullptr;

    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) {  // If in range
        // Search defenders
        for (pair<string, DataBattlePiece*> p : db->defenders) {
            for (ProgramSector* s : p.second->sectors) {
                if (s->coord == targetCoord) {
                    // We've found our target
                    target = p.second;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }
        // Search friendlies
        for (DataBattlePiece* p : db->friendlies) {
            for (ProgramSector* s : p->sectors) {
                if (s->coord == targetCoord) {
                    target = p;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }

        // Turn target invisible
        if (target != nullptr) {
            cout << "Granting invisibility\n";
            target->statuses['i']++;  // Increment the invisibility counter
            target->visible = false;
        }
    }
}

// Blip
Blip::Blip() {
    this->actionName = "Blip";
    this->description = "Blip \n Range: 5 \n Delete 1 sector from target";
    this->range = 5;
    this->targetSprite = 0;
}
Blip::~Blip() {}
void Blip::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    DataBattlePiece* target;

    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) {  // If in range
        // Search friendlies
        for (DataBattlePiece* p : db->friendlies) {
            for (ProgramSector* s : p->sectors) {
                if (s->coord == targetCoord) {
                    target = p;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }

        // Take damage
        if (target != nullptr) {
            target->takeDamage(db, 1);
        }
    }
}

// Thump
Thump::Thump() {
    this->actionName = "Thump";
    this->description = "Thump \n Delete 3 sectors from target";
    this->range = 1;
    this->targetSprite = 0;
}
Thump::~Thump() {}
void Thump::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    DataBattlePiece* target;

    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) {  // If in range
        // Search friendlies
        for (DataBattlePiece* p : db->friendlies) {
            for (ProgramSector* s : p->sectors) {
                if (s->coord == targetCoord) {
                    target = p;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }

        // Take damage
        if (target != nullptr) {
            target->takeDamage(db, 3);
        }
    }
}

// Burn
Burn::Burn() {
    this->actionName = "Burn";
    this->description = "Burn \n Delete 2 sectors from target";
    this->range = 1;
    this->targetSprite = 0;
}
Burn::~Burn() {}
void Burn::use(Netmap_Playable* db, DataBattlePiece* source, vector<sf::Vector2i> targets) {
    sf::Vector2i targetCoord = targets[0];
    sf::Vector2i origin = source->sectors[0]->coord;
    DataBattlePiece* target = nullptr;
    cout << "targetCoord = " << getByteCoord(targetCoord) << '\n';

    if (abs(targetCoord.x - origin.x) + abs(targetCoord.y - origin.y) <= this->range) {  // If in range
        // Search friendlies
        for (DataBattlePiece* p : db->friendlies) {
            cout << "Searching friendly " << p->name << '\n';
            for (ProgramSector* s : p->sectors) {
                cout << "Searching sector " << getByteCoord(s->coord) << '\n';
                if (s->coord == targetCoord) {
                    cout << "Hit target coord\n";
                    target = p;
                    break;
                }
            }
            if (target != nullptr) {
                break;
            }
        }

        // Take damage
        if (target != nullptr) {
            cout << "Dealing damage\n";
            target->takeDamage(db, 2);
        }
    }
}
*/
