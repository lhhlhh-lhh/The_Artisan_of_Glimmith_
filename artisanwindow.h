#ifndef ARTISANWINDOW_H
#define ARTISANWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QColor>
#include <QPainterPath>
#include <QSoundEffect>
#include <QMap>
#include <memory>
#include "LevelHandler.h" // 确保 GlassPiece 在这里定义

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
    Ui::ArtisanWindow* ui;

    // 策略模式：解决 E0020 错误的关键
    std::unique_ptr<LevelHandler> levelStrategy;

    // 数据容器：直接使用来自 LevelHandler.h 的 GlassPiece
    QList<GlassPiece> levelPieces;

    // 状态控制
    int currentLevel = 1;
    int currentPartId = 0;
    bool isLeftPressed = false;
    bool isRightPressed = false;
    QColor selectedColor = Qt::red;

    // 解决 0xC0000409 堆栈溢出
    bool isChecking = false;

    // 音效管理
    std::unique_ptr<QSoundEffect> paintSound;
    std::unique_ptr<QSoundEffect> winSound;

    // 内部函数：确保在 .cpp 中实现时带有 ArtisanWindow:: 前缀
    void loadLevel(int level);
    void checkWin();
    void handlePainting(QPoint pos, bool isEraser);
    void loadSounds();
};

#endif // ARTISANWINDOW_H