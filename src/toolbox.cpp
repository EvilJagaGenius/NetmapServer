// A useful place to put global functions
#include <toolbox.h>

bool startsWith(string str, string prefix) {
    return (0 == str.compare(0, prefix.length(), prefix));
}

bool endsWith(string str, string postfix) {
    if (str.size() >= postfix.size()) {
        return (0 == str.compare(str.length() - postfix.length(), postfix.length(), postfix));
    }
    return false;
}

vector<string> splitString(string str, char delimiter) {
    vector<string> results;
    istringstream stream(str);
    while (!stream.eof()) {
        string x;
        getline(stream, x, delimiter);
        results.push_back(x);
    }
    return results;
}

vector<sf::Vector2i> getRadius(int radius, sf::Vector2i origin, bool includeOrigin) {
    vector<sf::Vector2i> coords;
    int x = 0;
    while (x <= radius) {
        for (int y=0; y<(radius-x+1); y++) {
            if (x != 0 && y != 0) {
                coords.push_back(sf::Vector2<int>(origin.x + x, origin.y + y));
                coords.push_back(sf::Vector2<int>(origin.x + x, origin.y - y));
                coords.push_back(sf::Vector2<int>(origin.x - x, origin.y + y));
                coords.push_back(sf::Vector2<int>(origin.x - x, origin.y - y));
            } else if (x == 0 && y != 0) {
                coords.push_back(sf::Vector2<int>(origin.x + x, origin.y + y));
                coords.push_back(sf::Vector2<int>(origin.x + x, origin.y - y));
            } else if (x != 0 && y == 0) {
                coords.push_back(sf::Vector2<int>(origin.x + x, origin.y + y));
                coords.push_back(sf::Vector2<int>(origin.x - x, origin.y + y));
            }
        }
        x++;
    }
    if (includeOrigin) {
        coords.push_back(sf::Vector2<int>(origin.x, origin.y));
    }
    return coords;
}

string getByteCoord(sf::Vector2i coord) {
    string result = "";
    if (coord.x > 9) {
        result.push_back('A' + coord.x - 10);
    } else {
        result.push_back('0' + coord.x);
    }
    if (coord.y > 9) {
        result.push_back('A' + coord.y - 10);
    } else {
        result.push_back('0' + coord.y);
    }
    return result;
}

sf::Vector2i readByteCoord(string byteCoord) {
    sf::Vector2i coord = sf::Vector2<int>(-1, -1);
    char xChar = byteCoord[0];
    char yChar = byteCoord[1];
    if ((xChar - '0' >= 0) && (xChar - '0' <= 9)) {  // This should execute if xChar is numeric
        coord.x = xChar - '0';
    } else if ((xChar - 'A' >= 0) && (xChar - 'A') <= 5) {  // This should execute if it's a hex digit
        coord.x = 10 + xChar - 'A';
    }  // Otherwise, it's invalid input

    if ((yChar - '0' >= 0) && (yChar - '0' <= 9)) {
        coord.y = yChar - '0';
    } else if ((yChar - 'A' >= 0) && (yChar - 'A') <= 5) {
        coord.y = 10 + yChar - 'A';
    }

    return coord;
}
