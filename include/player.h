#ifndef PLAYER_H
#define PLAYER_H

#include "program.h"

#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>

using namespace std;

class Player
{
    public:
        // Variables
        unordered_map<string, int> programs;
        unordered_map<string, DataBattlePiece*> uploadMap;
        int credits;
        bool ready;
        string name;
        string color;  // Store the color as a hex value in a string instead of as an sf::Color
        string status; // If they're free, in a databattle, etc
        queue<string> cmdQueue;

        // Functions
        Player();
        virtual ~Player();
        void giveStartingPrograms();
        void readyup();
        void addToUploadMap(string byteCoord, string pieceName);
        void loadCharacter(string characterName);
        virtual string getNextCommand();
        virtual void sendMessage(string message);

    protected:

    private:
};

#endif // PLAYER_H
