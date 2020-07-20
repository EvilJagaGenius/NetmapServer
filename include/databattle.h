#ifndef DATABATTLE_H
#define DATABATTLE_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>

#include "toolbox.h"
#include "program.h"
#include "uploadzone.h"
#include "player.h"

// What is a databattle?
// It's... a game manager, in a sense.  That's what this should be.
// What should be handling player input and rendering?  I think we should create a new DataBattlePlayable class to interface between the game manager and the player.

using namespace std;

class DataBattle
{
    // Following my style, make everything public until needed otherwise
    public:
        // Variables
        string filename;
        string bkgFilename;
        string musicFilename;
        string destination;
        int grid[16][16];
        int currentPlayerIndex;
        vector<Player*> players;
        int currentProgramIndex;
        DataBattlePiece* currentProgram;
        DataBattlePiece* programStartingState;
        int nextProgramIndex;
        DataBattlePiece* nextProgram;
        sf::Vector2i programHead;
        vector<DataBattlePiece*> pieces;
        int pieceCounter;


        // Functions
        DataBattle();
        DataBattle(string filename);
        virtual ~DataBattle();
        void load();
        void addPlayer(Player* player);
        void switchTurns();
        void switchPrograms();
        void addPiece(DataBattlePiece* newPiece);
        void performAction(ProgramAction* action, vector<sf::Vector2i> targets);
        string takeCommand(string command, int playerIndex);
        string lookAt(sf::Vector2i coord);
        string lookAt(int x, int y);
        int checkForVictory();
        void flipSector(sf::Vector2i coord);
        void tick();

    protected:

    private:
};

//const static sf::Texture GRID_SHEET = imgLoad("Data\\Sprites\\Grid.png");

#endif // DATABATTLE_H
