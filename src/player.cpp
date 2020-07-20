#include "player.h"

Player::Player()
{
    //ctor

    this->giveStartingPrograms();
}

Player::~Player(){
    // Delete any entries still in uploadMap
    for (pair<string, DataBattlePiece*> p : this->uploadMap) {
        if (p.second != nullptr) {
            delete this->uploadMap[p.first];
        }
    }
}

void Player::giveStartingPrograms() {
    // Debug mode
    this->programs.insert({{"Hack", 10},
                          {"Slingshot", 10},
                          {"DataDr", 10},
                          {"BitMan", 10},
                          {"Bug", 10},
                          {"Watchman", 10}});
}

void Player::readyup() {
    this->ready = true;
}

void Player::addToUploadMap(string byteCoord, string pieceName) {
    // Put the piece in uploadMap, so we upload it on hitting DBI
    cout << "Adding piece to upload map\n";

    DataBattlePiece* newPiece = new Program(pieceName);

    // See if we already have a piece ready to upload at that coord
    bool foundUpload = false;
    for (pair<string, DataBattlePiece*> p : this->uploadMap) {
        if (p.first == byteCoord) {  // If so
            cout << "Replacing piece\n";
            // Replace it
            foundUpload = true;
            delete this->uploadMap[p.first];
            this->uploadMap[p.first] = newPiece;
            break;
        }
    }
    if (!foundUpload) {  // Otherwise
        cout << "Creating spot in upload map\n";
        // Create an entry for that spot in uploadMap
        this->uploadMap.emplace(byteCoord, newPiece);
    }
}
