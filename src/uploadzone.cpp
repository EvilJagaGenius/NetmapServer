#include "uploadzone.h"

UploadZone::UploadZone() {
    //ctor
}

UploadZone::UploadZone(int x, int y, int owner) {
    this->owner = owner;
    this->controller = owner;
    this->pieceType = 'u';
    // I'm not sure whether we should use ProgramSectors or not.  I guess I'll use them for consistency's sake
    this->sectors.push_back(new ProgramSector(sf::Vector2<int>(x, y)));
    this->size = 1;
    this->spriteCoord = sf::Vector2<int>(0, 2);
}

UploadZone::~UploadZone() {
    //dtor
}
