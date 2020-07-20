#ifndef PROGRAMSECTOR_H
#define PROGRAMSECTOR_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "toolbox.h"

class ProgramSector
{
    public:
        sf::Vector2i coord;
        vector<ProgramSector*> links;
        int numLinks;
        bool fortified;

        ProgramSector(sf::Vector2i coord);
        ProgramSector(sf::Vector2i coord, ProgramSector* lastSector);
        virtual ~ProgramSector();
        static void linkSectors(ProgramSector* a, ProgramSector* b);

    protected:

    private:
};

#endif // PROGRAMSECTOR_H
