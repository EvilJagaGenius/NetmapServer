#ifndef UPLOADZONE_H
#define UPLOADZONE_H

#include "databattlepiece.h"

class UploadZone : public DataBattlePiece
{
    public:
        UploadZone();
        UploadZone(int x, int y, int owner);
        virtual ~UploadZone();

    protected:

    private:
};

#endif // UPLOADZONE_H
