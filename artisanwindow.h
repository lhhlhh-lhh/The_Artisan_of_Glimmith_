#ifndef ARTISANWINDOW_H
#define ARTISANWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QColor>
#include <QPainterPath>
#include <QSoundEffect>
#include <memory>

struct GlassPiece {
    QPainterPath path;
    QColor color;
    QColor targetColor;
};

QT_BEGIN_NAMESPACE
namespace Ui { class ArtisanWindow; }
QT_END_NAMESPACE

class ArtisanWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ArtisanWindow(QWidget* parent = nullptr);
    ~ArtisanWindow() override; // 必须有且仅有一个实现

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    Ui::ArtisanWindow* ui;
    QList<GlassPiece> levelPieces;
    int currentLevel = 1;
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