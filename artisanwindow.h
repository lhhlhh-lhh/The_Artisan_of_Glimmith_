#ifndef ARTISANWINDOW_H
#define ARTISANWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QColor>
#include <QPainterPath>
#include <QSoundEffect>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class ArtisanWindow; }
QT_END_NAMESPACE

class ArtisanWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ArtisanWindow(QWidget* parent = nullptr);
    ~ArtisanWindow() override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    // 将结构体移入私有区域，并增加逻辑判定所需的成员
    struct GlassPiece {
        QPainterPath path;
        QColor color = Qt::white;
        QColor targetColor = Qt::white;
        int row;         // 记录所在的行
        int col;         // 记录所在的列
        int partId = -1; // 记录笔画ID，-1为未涂色，相同正数代表同一次画出的连续部分
    };

    Ui::ArtisanWindow* ui;
    QList<GlassPiece> levelPieces;
    int currentLevel = 1;
    int currentPartId = 0;   // 笔画计数器，用于区分不同的正方形笔迹

    bool isLeftPressed = false;
    bool isRightPressed = false;
    QColor selectedColor = Qt::red;

    std::unique_ptr<QSoundEffect> paintSound;
    std::unique_ptr<QSoundEffect> winSound;

    void loadLevel(int level);
    void checkWin();
    void handlePainting(QPoint pos, bool isEraser);
    void loadSounds();
};

#endif