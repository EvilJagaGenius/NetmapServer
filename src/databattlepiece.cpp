#include "databattlepiece.h"

DataBattlePiece::DataBattlePiece()
{
    this->statuses =   {{'g', 0},  // Gridwalk.  Move anywhere on the grid regardless of it being a tile or not
                        {'b', 0},  // Gridburn.  Each sector deleted from a tile sets that grid sector to empty
                        {'n', 0},  // Negawalk.  Only move on blank grid sectors
                        {'i', 0},  // Invisibility.
                        {'a', 0},  // Alarm.  Attacks made on you deal an extra point of damage
                        {'t', 0},  // Traced.  Can't turn invisible and any mines you leave will be visible
                        {'w', 0},  // Webbed.  -1 speed debuff
                        {'f', 0},  // Frozen.  Speed locked to 0
                        {'s', 0},  // Shutdown.  Lose a turn
                        {'l', 0},  // Locked.  Can't warp or teleport
                        {'c', 0},  // Capped.  Can't grow/heal
                        {'p', 0},  // Poison.  Take a point of damage on finishing the turn
                        {'h', 0}}; // Hacked.  Another player has taken control of the piece
}

DataBattlePiece::~DataBattlePiece()
{
    //dtor
}
void DataBattlePiece::deleteSectors() {}
void DataBattlePiece::load() {}
void DataBattlePiece::move(sf::Vector2i coord, bool firstTime) {}
void DataBattlePiece::addSector(sf::Vector2i coord, int pos) {}
void DataBattlePiece::useAction(Netmap_Playable* level, int actionIndex, vector<sf::Vector2i> targets) {}
void DataBattlePiece::switchToAiming() {}
void DataBattlePiece::noAction() {}
void DataBattlePiece::takeDamage(int damage) {}
/*void DataBattlePiece::amputate(Netmap_Playable* level, sf::Vector2i coord) {}
void DataBattlePiece::grow(Netmap_Playable* level, int amtToGrow) {
    cout << "Called DataBattlePiece::grow()\n";
}*/
void DataBattlePiece::prepForTurn() {}
void DataBattlePiece::tickStatuses() {  // This requires a dedicated function
    // The behavior for status effects should stay the same no matter what program type, so I hope this works
    for (pair<char, int> p : this->statuses) {
        if (p.second > 0) {
            if (p.first == 'p') {  // Poison
                // Deal one damage to self
                //this->takeDamage(1);  // I know there's supposed to be a Netmap_Playable there.  We should fix this
                cout << "Poison damage\n";
                // We need some way to pass this information to the databattle
            }
            cout << "Decrementing status " << p.first << '\n';
            p.second--;
        }
    }
}
