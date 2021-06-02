#ifndef PROGRAM_H
#define PROGRAM_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <unordered_map>

#include "databattlepiece.h"
#include "programsector.h"
#include "programaction.h"

#include "toolbox.h"

using namespace std;

class DataBattle;  // Used to break a circular dependency
class ProgramAction;
class Program: public DataBattlePiece
{
    public:
        // See DataBattlePiece for the rest of the variables
        int cost;

        // Functions
        Program(string programType);
        Program(Program* original);  // Copy constructor
        Program(DataBattlePiece* original);  // Alt copy constructor
        virtual ~Program();
        void deleteSectors();
        void load();
        void move(sf::Vector2i coord, bool firstTime);  // Another thing that might be reserved in C++.  Maybe make this bool?
        void addSector(sf::Vector2i coord, int pos);
        void switchToAiming();
        void noAction();
        void takeDamage(int damage);
        //void amputate(Netmap_Playable* level, sf::Vector2i coord);
        //void grow(Netmap_Playable* level, int amtToGrow);
        void prepForTurn();

    protected:

    private:
};


// Constants
//const static sf::Texture PROGRAM_SHEET = imgLoad("Data\\Sprites\\Programs.png");
// I'm not entirely sure we need PROGRAM_DB (bad memory practice and all), but I'm keeping it around for the time being
/*static unordered_map<string, Program*> PROGRAM_DB = {{"Hack", new Program("Hack")},
                                                    {"Slingshot", new Program("Slingshot")},
                                                    {"DataDr", new Program("DataDr")},
                                                    {"Bug", new Program("Bug")},
                                                    {"BitMan", new Program("BitMan")},
                                                    {"Sentinel", new Program("Sentinel")},
                                                    {"GuardDog", new Program("GuardDog")},
                                                    {"Watchman", new Program("Watchman")},
                                                    {"Sensor", new Program("Sensor")},
                                                    {"Warden", new Program("Warden")},
                                                    {"FireWall", new Program("FireWall")}};
*/

#endif // PROGRAM_H
