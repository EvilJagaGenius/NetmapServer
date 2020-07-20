#include <iostream>
#include <string>
#include <SFML/Network.hpp>

using namespace std;

int main(int argc, char** argv) {
    for (int i=0; i<argc; i++) {
        cout << argv[i];
    }
    cout << '\n';

    int PORT = 9900;
    // Do something, Taipu
    // Open a port
    sf::TcpListener listener;
    listener.listen(PORT);
    cout << "Listening on port " << PORT << '\n';

    while (true) {
        sf::TcpSocket client;
        if (listener.accept(client) == sf::Socket::Done) {
            cout << "New connection received from " << client.getRemoteAddress() << std::endl;
        }
    }
}
