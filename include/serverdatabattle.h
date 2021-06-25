#ifndef SERVERDATABATTLE_H
#define SERVERDATABATTLE_H

#include <SFML/Network.hpp>

#include "databattle.h"
#include "networkplayer.h"
// I'm not sure if we'll eventually merge this with the normal DataBattle class.  We'll see.

class ServerDataBattle : public DataBattle
{
    public:
        int creditLimit;
        bool shop;
        bool characters;

        ServerDataBattle();
        ServerDataBattle(string filename);
        virtual ~ServerDataBattle();
        void tick();
        string takeCommand(string command, int playerIndex);
        void broadcast(string command);
        void switchPrograms();
        void switchTurns();
        void addPlayer(NetworkPlayer* player);

    protected:

    private:
};

#endif // SERVERDATABATTLE_H
