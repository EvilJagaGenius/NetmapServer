#ifndef NETWORKPLAYER_H
#define NETWORKPLAYER_H

#include <player.h>


class NetworkPlayer : public Player
{
    public:
        sf::TcpSocket* socket;
        string status; // If they're free, in a databattle, etc

        NetworkPlayer();
        virtual ~NetworkPlayer();
        string getNextCommand();
        void sendMessage(string message);

    protected:

    private:
};

#endif // NETWORKPLAYER_H
