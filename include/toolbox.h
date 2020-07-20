// A useful place to put global functions
#ifndef TOOLBOX_H_INCLUDED
#define TOOLBOX_H_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <ctype.h>

#include <SFML/Network.hpp>

using namespace std;

const static int TILE_SIZE = 32;
const static int GAP_SIZE = 4;

const static int WX = 1024;
const static int WY = 576;

bool startsWith(string str, string prefix);
bool endsWith(string str, string postfix);
vector<string> splitString(string str, char delimiter);
string getByteCoord(sf::Vector2i coord);
sf::Vector2i readByteCoord(string byteCoord);
vector<sf::Vector2i> getRadius(int radius, sf::Vector2i origin, bool includeOrigin=false);


#endif // TOOLBOX_H_INCLUDED
