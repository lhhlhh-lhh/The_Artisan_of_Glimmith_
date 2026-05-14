#ifndef LEVELHANDLER_H
#define LEVELHANDLER_H

#include <QList>
#include <QPainterPath>
#include <QColor>
#include <QString> // 必须包含这个头文件来使用 QString

// 保持 GlassPiece 结构体不变
struct GlassPiece {
    QPainterPath path;
    QColor color = Qt::white;
    int row = 0;
    int col = 0;
    int partId = -1;
};

class LevelHandler {
public:
    virtual ~LevelHandler() = default;
    virtual void loadLevel(QList<GlassPiece>& pieces, int w, int h) = 0;
    virtual bool checkWin(const QList<GlassPiece>& pieces) = 0;

    
    virtual QString getHint() const = 0;
};

#endif