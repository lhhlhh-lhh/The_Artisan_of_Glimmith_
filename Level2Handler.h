#ifndef LEVEL2HANDLER_H
#define LEVEL2HANDLER_H

#include "LevelHandler.h"

class Level2Handler : public LevelHandler {
public:
    void loadLevel(QList<GlassPiece>& pieces, int screenWidth, int screenHeight) override;
    bool checkWin(const QList<GlassPiece>& pieces) override;
};

#endif // LEVEL2HANDLER_H