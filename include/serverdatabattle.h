#ifndef SERVERDATABATTLE_H
#define SERVERDATABATTLE_H

#include <SFML/Network.hpp>

#include "databattle.h"
#include "networkplayer.h"
// I'm not sure if we'll eventually merge this with the normal DataBattle class.  We'll see.

class ServerDataBattle : public DataBattle
{
    public:
        vector<sf::TcpSocket*> clientSockets;
        sf::TcpListener* listener;
        int port;

        ServerDataBattle();
        ServerDataBattle(string filename);
        virtual ~ServerDataBattle();
        void tick();

    protected:

    private:
};

#endif // SERVERDATABATTLE_H
