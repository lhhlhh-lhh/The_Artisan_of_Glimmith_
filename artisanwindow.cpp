#include "artisanwindow.h"
#include "ui_artisanwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QCoreApplication>
#include <QUrl>

ArtisanWindow::ArtisanWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::ArtisanWindow) {
    ui->setupUi(this);
    setFixedSize(800, 600);
    loadSounds();
    loadLevel(currentLevel);
    setMouseTracking(true);
}

// 注意：整个文件只能有一个析构函数！
ArtisanWindow::~ArtisanWindow() {
    delete ui;
}

void ArtisanWindow::loadLevel(int level) {
    levelPieces.clear();
    int size = 60, startX = 100, startY = 150;
    if (level == 1) {
        for (int i = 0; i < 5; ++i) {
            GlassPiece p;
            p.path.addRect(startX + i * size, startY, size, size);
            p.color = Qt::white;
            p.targetColor = Qt::red;
            levelPieces << p;
        }
    }
    else if (level == 2) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                GlassPiece p;
                p.path.addRect(startX + i * size, startY + j * size, size, size);
                p.color = Qt::white;
                p.targetColor = (i == 1 || j == 1) ? Qt::green : Qt::white;
                levelPieces << p;
            }
        }
    }
    else {
        QMessageBox::information(this, "Game Over", "You are a master artisan!");
        currentLevel = 1; loadLevel(1);
    }
    update();
}

void ArtisanWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (event->pos().x() > 650) {
            if (selectedColor == Qt::red) selectedColor = Qt::green;
            else if (selectedColor == Qt::green) selectedColor = Qt::blue;
            else selectedColor = Qt::red;
            update();
        }
        else {
            isLeftPressed = true;
            handlePainting(event->pos(), false);
        }
    }
    else if (event->button() == Qt::RightButton) {
        isRightPressed = true;
        handlePainting(event->pos(), true);
    }
}

void ArtisanWindow::mouseMoveEvent(QMouseEvent* event) {
    if (isLeftPressed) handlePainting(event->pos(), false);
    else if (isRightPressed) handlePainting(event->pos(), true);
}

void ArtisanWindow::mouseReleaseEvent(QMouseEvent* event) {
    isLeftPressed = false;
    isRightPressed = false;
    checkWin();
}

void ArtisanWindow::handlePainting(QPoint pos, bool isEraser) {
    QColor colorToApply = isEraser ? Qt::white : selectedColor;
    for (int i = 0; i < levelPieces.size(); ++i) {
        if (levelPieces[i].path.contains(pos)) {
            if (levelPieces[i].color != colorToApply) {
                levelPieces[i].color = colorToApply;
                if (paintSound) {
                    paintSound->stop();
                    paintSound->play();
                }
                update();
            }
            break;
        }
    }
}

void ArtisanWindow::checkWin() {
    bool allCorrect = true;
    for (const auto& p : levelPieces) {
        if (p.color != p.targetColor) { allCorrect = false; break; }
    }
    if (allCorrect) {
        if (winSound) {
            winSound->stop();
            winSound->play();
        }
        QMessageBox::information(this, "Success", "Level Clear!");
        currentLevel++; loadLevel(currentLevel);
    }
}

void ArtisanWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(40, 40, 40));
    for (const auto& p : levelPieces) {
        painter.setBrush(p.color);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawPath(p.path);
    }
    painter.setBrush(QColor(80, 80, 80));
    painter.drawRect(650, 0, 150, height());
    painter.setPen(Qt::white);
    painter.drawText(670, 50, QString("Level: %1").arg(currentLevel));
    painter.setBrush(selectedColor);
    painter.drawRect(680, 100, 50, 50);
}

void ArtisanWindow::loadSounds() {
    paintSound = std::make_unique<QSoundEffect>();
    winSound = std::make_unique<QSoundEffect>();

    paintSound->setSource(QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/sounds/paint.wav"));
    paintSound->setVolume(1.0f);

    winSound->setSource(QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/sounds/win.wav"));
    winSound->setVolume(1.0f);
}