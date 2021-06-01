#ifndef LOBBY_H
#define LOBBY_H

#include <vector>
#include "serverdatabattle.h"
#include "networkplayer.h"

class Lobby
{
    public:
        vector<NetworkPlayer*> players;
        //vector<string> statuses;  // If the player's free, in a DB, etc
        int maxDataBattles;
        vector<ServerDataBattle*> dbs;  // The server's list of databattles.  nullptr if there's no DB in that slot
        int port;
        sf::TcpListener* listener;

        Lobby();
        Lobby(int portNum);
        virtual ~Lobby();
        void tick();
        void welcomePlayer(NetworkPlayer* player);
        void tickCleanup();
        string takeCommand(string command, int playerIndex);
        void broadcast(string command);

    protected:

    private:
};

#endif // LOBBY_H
