#include "artisanwindow.h"
#include "ui_artisanwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QStyleOption>
#include <QDebug>
#include <algorithm>

// =================================================================
// 关卡 1 逻辑：标准 8x8，寻找 4 个 4x4 的正方形
// =================================================================
class Level1Handler : public LevelHandler {
public:
    void loadLevel(QList<GlassPiece>& pieces, int w, int h) override {
        int rows = 8, cols = 8, size = 50;
        int startX = (650 - cols * size) / 2;
        int startY = (h - rows * size) / 2;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                GlassPiece p;
                p.row = r; p.col = c;
                p.path.addRect(startX + c * size, startY + r * size, size, size);
                pieces << p;
            }
        }
    }

    bool checkWin(const QList<GlassPiece>& pieces) override {
        QMap<int, QList<int>> groups;
        for (int i = 0; i < pieces.size(); ++i) {
            if (pieces[i].partId != -1) groups[pieces[i].partId].append(i);
        }

        int validSquares = 0;
        for (auto it = groups.constBegin(); it != groups.constEnd(); ++it) {
            const QList<int>& indices = it.value();
            if (indices.size() != 16) continue;

            int minR = 99, maxR = -1, minC = 99, maxC = -1;
            for (int idx : indices) {
                minR = qMin(minR, pieces[idx].row); maxR = qMax(maxR, pieces[idx].row);
                minC = qMin(minC, pieces[idx].col); maxC = qMax(maxC, pieces[idx].col);
            }
            if ((maxR - minR + 1) == 4 && (maxC - minC + 1) == 4) validSquares++;
        }
        return (validSquares == 4);
    }
    // 新增提示
    QString getHint() const override { return "目标：\n将画面划分为\n4个 4x4 的大正方形"; }
};

// =================================================================
// 关卡 2 逻辑：不规则 4x6，Domino 与 L-shape 判定
// =================================================================
class Level2Handler : public LevelHandler {
public:
    void loadLevel(QList<GlassPiece>& pieces, int w, int h) override {
        int rows = 6, cols = 4, size = 55;
        int startX = (650 - cols * size) / 2;
        int startY = (h - rows * size) / 2;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if ((c < 1 && r < 2) || (c >= 2 && r < 1) ||
                    (c < 2 && r >= 3) || (c >= 3 && r >= 2)) continue;

                GlassPiece p;
                p.row = r; p.col = c;
                p.path.addRect(startX + c * size, startY + r * size, size, size);
                pieces << p;
            }
        }
    }

    bool checkWin(const QList<GlassPiece>& pieces) override {
        QMap<int, QList<GlassPiece>> groups;
        for (const auto& p : pieces) {
            if (p.partId == -1) return false;
            groups[p.partId].append(p);
        }

        for (auto it = groups.constBegin(); it != groups.constEnd(); ++it) {
            const QList<GlassPiece>& group = it.value();
            int n = group.size();
            int minR = 9, maxR = -1, minC = 9, maxC = -1;
            for (const auto& p : group) {
                minR = qMin(minR, p.row); maxR = qMax(maxR, p.row);
                minC = qMin(minC, p.col); maxC = qMax(maxC, p.col);
            }
            int width = maxC - minC + 1, height = maxR - minR + 1;
            bool isDomino = (n == 2 && ((width == 1 && height == 2) || (width == 2 && height == 1)));
            bool isLShape = (n == 3 && width == 2 && height == 2);
            if (!isDomino && !isLShape) return false;
        }
        return true;
    }
    // 新增提示
    QString getHint() const override { return "目标：\n区域仅限 1x2 长方形\n或 3格组成的 L型"; }
};
class Level3Handler : public LevelHandler {
public:
    void loadLevel(QList<GlassPiece>& pieces, int w, int h) override {
        int rows = 7, cols = 7, size = 50; // 奇数行格，中心对称点更好找
        int startX = (650 - cols * size) / 2;
        int startY = (h - rows * size) / 2;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                GlassPiece p; p.row = r; p.col = c;
                p.path.addRect(startX + c * size, startY + r * size, size, size);
                pieces << p;
            }
        }
    }

    bool checkWin(const QList<GlassPiece>& pieces) override {
        QMap<int, QList<GlassPiece>> groups;
        for (const auto& p : pieces) {
            if (p.partId == -1) return false;
            groups[p.partId].append(p);
        }

        for (auto group : groups) {
            if (group.size() < 2) continue;
            // 简单的判定：如果区域的Bounding Box是奇数宽度，检查中心对称
            // 这里我们放宽要求：每个区域必须包含 5 或 6 个格子的长方形
            int n = group.size();
            if (n != 5 && n != 6) return false;
        }
        return true;
    }
    QString getHint() const override { return "目标：\n每个区域必须由\n5个或6个格子组成"; }
};
class Level4Handler : public LevelHandler {
public:
    void loadLevel(QList<GlassPiece>& pieces, int w, int h) override {
        int rows = 10, cols = 10, size = 45;
        int startX = (650 - cols * size) / 2;
        int startY = (h - rows * size) / 2;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                // 挖掉中间的 2x2 核心
                if (r >= 4 && r <= 5 && c >= 4 && c <= 5) continue;

                GlassPiece p; p.row = r; p.col = c;
                p.path.addRect(startX + c * size, startY + r * size, size, size);
                pieces << p;
            }
        }
    }

    bool checkWin(const QList<GlassPiece>& pieces) override {
        QMap<int, QList<GlassPiece>> groups;
        for (const auto& p : pieces) {
            if (p.partId == -1) return false;
            groups[p.partId].append(p);
        }
        // 胜利条件：所有区域格数必须是 3 的倍数（模拟色彩三原色组合）
        for (auto group : groups) {
            if (group.size() % 3 != 0) return false;
        }
        return true;
    }
    QString getHint() const override { return "目标：\n三位一体！\n每个区域的格子数\n必须是 3 的倍数"; }
};


// =================================================================
// ArtisanWindow 成员函数
// =================================================================
ArtisanWindow::ArtisanWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::ArtisanWindow), currentLevel(1), currentPartId(0), isChecking(false)
{
    ui->setupUi(this);
    setFixedSize(800, 600);

    // --- 修复背景图 ---
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet(
        "QMainWindow {"
        "  background-image: url(:/images/paper_bg.png);"
        "  background-repeat: no-repeat;"
        "  background-position: center;"
        "  background-color: #f0e6d2;"
        "}"
    );

    loadSounds();
    loadLevel(currentLevel);
    setMouseTracking(true);
}

ArtisanWindow::~ArtisanWindow() { delete ui; }

void ArtisanWindow::loadLevel(int level) {
    levelPieces.clear();
    currentPartId = 0;

    if (level == 1) {
        levelStrategy = std::make_unique<Level1Handler>();
    }
    else if (level == 2) {
        levelStrategy = std::make_unique<Level2Handler>();
    }
    else if (level == 3) {
        levelStrategy = std::make_unique<Level3Handler>();
    }
    else if (level == 4) {
        levelStrategy = std::make_unique<Level4Handler>();
    }
    else {
        QMessageBox::information(this, "恭喜！", "你已经完成了所有挑战.");
        currentLevel = 1;
        loadLevel(1);
        return;
    }

    if (levelStrategy) levelStrategy->loadLevel(levelPieces, width(), height());
    update();
}

void ArtisanWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // --- 核心修复：工具栏颜色切换逻辑 ---
        if (event->pos().x() > 650) {
            int y = event->pos().y();
            int yOffset = 320; // 必须与 paintEvent 里的 yOffset 保持绝对一致

            // 判定每一个 30x30 的色块区域（给点击增加一点冗余，设为 40 高度）
            if (y >= yOffset && y < yOffset + 40) {
                selectedColor = Qt::red;
            }
            else if (y >= yOffset + 50 && y < yOffset + 90) {
                selectedColor = Qt::blue;
            }
            else if (y >= yOffset + 100 && y < yOffset + 140) {
                selectedColor = Qt::green;
            }
            else if (y >= yOffset + 150 && y < yOffset + 190) {
                selectedColor = Qt::yellow;
            }

            update();
            return;
        }


        isLeftPressed = true;
        currentPartId++;
        handlePainting(event->pos(), false);
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
    if (event->button() == Qt::LeftButton) {
        isLeftPressed = false;
        checkWin();
    }
    else if (event->button() == Qt::RightButton) {
        isRightPressed = false;
    }
}

void ArtisanWindow::handlePainting(QPoint pos, bool isEraser) {
    if (isChecking || levelPieces.isEmpty()) return;
    QColor colorToApply = isEraser ? Qt::white : selectedColor;
    for (int i = 0; i < levelPieces.size(); ++i) {
        if (levelPieces[i].path.contains(pos)) {
            if (levelPieces[i].color != colorToApply) {
                levelPieces[i].color = colorToApply;
                levelPieces[i].partId = isEraser ? -1 : currentPartId;
                if (paintSound && paintSound->isLoaded()) paintSound->play();
                update();
            }
            break;
        }
    }
}

void ArtisanWindow::checkWin() {
    if (isChecking || levelPieces.isEmpty() || !levelStrategy) return;
    isChecking = true;
    if (levelStrategy->checkWin(levelPieces)) {
        if (winSound && winSound->isLoaded()) winSound->play();
        QMessageBox::information(this, "Masterpiece!", QString("Level %1 Clear!").arg(currentLevel));
        currentLevel++;
        loadLevel(currentLevel);
    }
    isChecking = false;
}

void ArtisanWindow::paintEvent(QPaintEvent* event) {
    if (isChecking || levelPieces.isEmpty()) return;

    QStyleOption opt; opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (const auto& piece : levelPieces) {
        painter.setBrush(piece.color);
        painter.setPen(QPen(QColor(60, 40, 20), 2));
        painter.drawPath(piece.path);
    }

    painter.setPen(QPen(QColor(255, 215, 0), 4));
    for (int i = 0; i < levelPieces.size(); ++i) {
        for (int j = 0; j < levelPieces.size(); ++j) {
            const auto& p1 = levelPieces[i], & p2 = levelPieces[j];
            if (p1.partId != p2.partId) {
                if (p2.col == p1.col + 1 && p2.row == p1.row)
                    painter.drawLine(p1.path.boundingRect().topRight(), p1.path.boundingRect().bottomRight());
                if (p2.row == p1.row + 1 && p2.col == p1.col)
                    painter.drawLine(p1.path.boundingRect().bottomLeft(), p1.path.boundingRect().bottomRight());
            }
        }
    }

    // 3. 绘制右侧工具栏
    painter.setBrush(QColor(0, 0, 0, 80));
    painter.setPen(Qt::NoPen);
    painter.drawRect(650, 0, 150, height());

    painter.setPen(QColor(255, 255, 255));
    QFont font = painter.font(); font.setBold(true); font.setPointSize(12);
    painter.setFont(font);
    painter.drawText(670, 50, QString("Level: %1").arg(currentLevel));

    // --- 动态绘制提示文字 ---
    if (levelStrategy) {
        painter.setPen(QColor(255, 255, 200));
        font.setPointSize(10); painter.setFont(font);
        painter.drawText(QRect(660, 200, 130, 100), Qt::AlignLeft | Qt::TextWordWrap, levelStrategy->getHint());
    }

    painter.setPen(QColor(255, 255, 255));
    font.setPointSize(12); painter.setFont(font);
    painter.drawText(670, 90, "Current Color:");

    painter.setBrush(selectedColor);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(680, 110, 50, 50);

    // 6. 选色面板（位置稍微下移，避开提示文字）
    int yOffset = 320;
    painter.setBrush(Qt::red); painter.drawRect(680, yOffset, 30, 30);
    painter.setBrush(Qt::blue); painter.drawRect(680, yOffset + 50, 30, 30);
    painter.setBrush(Qt::green); painter.drawRect(680, yOffset + 100, 30, 30);
    painter.setBrush(Qt::yellow); painter.drawRect(680, yOffset + 150, 30, 30);
}

void ArtisanWindow::loadSounds() {
    paintSound = std::make_unique<QSoundEffect>(this);
    winSound = std::make_unique<QSoundEffect>(this);
    paintSound->setSource(QUrl("qrc:/sounds/paint.wav"));
    winSound->setSource(QUrl("qrc:/sounds/win.wav"));
}