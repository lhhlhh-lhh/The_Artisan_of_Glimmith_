#include "artisanwindow.h"
#include "ui_artisanwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QStyleOption>
#include <QDebug>
#include <algorithm>
// 第一部分：关卡策略类 (Strategy Pattern)
// 每个类继承自 LevelHandler，实现特定的关卡初始化与胜负判定逻辑
// --- 关卡 1 逻辑：标准 8x8，寻找 4 个 4x4 的正方形 ---
class Level1Handler : public LevelHandler {
public:
    void loadLevel(QList<GlassPiece>& pieces, int w, int h) override {
        int rows = 8, cols = 8, size = 50;
        int startX = (650 - cols * size) / 2; // 居中计算
        int startY = (h - rows * size) / 2;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                GlassPiece p;
                p.row = r; p.col = c;
                // 记录矩形物理路径，用于后续的 QPainter 绘制与 QMouseEvent 碰撞检测
                p.path.addRect(startX + c * size, startY + r * size, size, size);
                pieces << p;
            }
        }
    }

    bool checkWin(const QList<GlassPiece>& pieces) override {
        QMap<int, QList<int>> groups; // 按 partId 将所有格子的索引进行分组
        for (int i = 0; i < pieces.size(); ++i) {
            if (pieces[i].partId != -1) groups[pieces[i].partId].append(i);
        }

        int validSquares = 0;
        for (auto it = groups.constBegin(); it != groups.constEnd(); ++it) {
            const QList<int>& indices = it.value();
            if (indices.size() != 16) continue; // 4x4 必须正好 16 格

            // 计算该区域的包围盒（Bounding Box）
            int minR = 99, maxR = -1, minC = 99, maxC = -1;
            for (int idx : indices) {
                minR = qMin(minR, pieces[idx].row); maxR = qMax(maxR, pieces[idx].row);
                minC = qMin(minC, pieces[idx].col); maxC = qMax(maxC, pieces[idx].col);
            }
            // 判定：如果行跨度与列跨度均为 4，说明构成了一个 4x4 的连续正方形
            if ((maxR - minR + 1) == 4 && (maxC - minC + 1) == 4) validSquares++;
        }
        return (validSquares == 4);
    }
    QString getHint() const override { return "目标：\n将画面划分为\n4个 4x4 的大正方形"; }
};

// --- 关卡 2 逻辑：不规则 4x6，Domino（2格）与 L-shape（3格）判定 ---
class Level2Handler : public LevelHandler {
public:
    void loadLevel(QList<GlassPiece>& pieces, int w, int h) override {
        int rows = 6, cols = 4, size = 55;
        int startX = (650 - cols * size) / 2;
        int startY = (h - rows * size) / 2;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                // 手动挖去部分格子，形成非矩形的游戏画布布局
                if ((c < 1 && r < 2) || (c >= 2 && r < 1) ||
                    (c < 2 && r >= 3) || (c >= 3 && r >= 2)) continue;

                GlassPiece p; p.row = r; p.col = c;
                p.path.addRect(startX + c * size, startY + r * size, size, size);
                pieces << p;
            }
        }
    }

    bool checkWin(const QList<GlassPiece>& pieces) override {
        QMap<int, QList<GlassPiece>> groups;
        for (const auto& p : pieces) {
            if (p.partId == -1) return false; // 所有格子必须都有归属
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
            // 2格的长方形或占地 2x2 空间的 3格（即 L型）
            bool isDomino = (n == 2 && ((width == 1 && height == 2) || (width == 2 && height == 1)));
            bool isLShape = (n == 3 && width == 2 && height == 2);
            if (!isDomino && !isLShape) return false;
        }
        return true;
    }
    QString getHint() const override { return "目标：\n区域仅限 1x2 长方形\n或 3格组成的 L型"; }
};

// 关卡 3 ：7x7 奇数网格，固定面积判定 
class Level3Handler : public LevelHandler {
public:
    void loadLevel(QList<GlassPiece>& pieces, int w, int h) override {
        int rows = 7, cols = 7, size = 50;
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
            int n = group.size();
            // 每个连通域面积必须精确为 5 或 6
            if (n != 5 && n != 6) return false;
        }
        return true;
    }
    QString getHint() const override { return "目标：\n每个区域必须由\n5个或6个格子组成"; }
};

// --- 关卡 4 逻辑：10x10 镂空网格，数论（3的倍数）判定 ---
class Level4Handler : public LevelHandler {
public:
    void loadLevel(QList<GlassPiece>& pieces, int w, int h) override {
        int rows = 10, cols = 10, size = 45;
        int startX = (650 - cols * size) / 2;
        int startY = (h - rows * size) / 2;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                // 挖掉正中心的 2x2 核心
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
        // 数论判定：所有区域格数必须能被 3 整除
        for (auto group : groups) {
            if (group.size() % 3 != 0) return false;
        }
        return true;
    }
    QString getHint() const override { return "目标：\n三位一体！\n每个区域的格子数\n必须是 3 的倍数"; }
};
// 第二部分：主窗口控制器实现 (ArtisanWindow)
// 负责资源加载、事件分发及核心渲染循环
ArtisanWindow::ArtisanWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::ArtisanWindow), currentLevel(1), currentPartId(0), isChecking(false)
{
    ui->setupUi(this);
    setFixedSize(800, 600);

    // 1. UI 视觉初始化
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
    loadLevel(currentLevel); // 加载首关
    setMouseTracking(true);   // 开启鼠标追踪以支持流畅绘画
}

ArtisanWindow::~ArtisanWindow() { delete ui; }

// 根据关卡 ID 实例化对应的处理器
void ArtisanWindow::loadLevel(int level) {
    levelPieces.clear();
    currentPartId = 0;

    if (level == 1) levelStrategy = std::make_unique<Level1Handler>();
    else if (level == 2) levelStrategy = std::make_unique<Level2Handler>();
    else if (level == 3) levelStrategy = std::make_unique<Level3Handler>();
    else if (level == 4) levelStrategy = std::make_unique<Level4Handler>();
    else {
        QMessageBox::information(this, "Masterpiece!", "恭喜！你已经完成了所有挑战.");
        currentLevel = 1; loadLevel(1); return;
    }

    if (levelStrategy) levelStrategy->loadLevel(levelPieces, width(), height());
    update();
}

// 2. 交互逻辑实现
void ArtisanWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // 侧边工具栏点击判定
        if (event->pos().x() > 650) {
            int y = event->pos().y();
            int yOffset = 320;
            // 判定颜色切换 (考虑了 10px 的间距)
            if (y >= yOffset && y < yOffset + 40) selectedColor = Qt::red;
            else if (y >= yOffset + 50 && y < yOffset + 90) selectedColor = Qt::blue;
            else if (y >= yOffset + 100 && y < yOffset + 140) selectedColor = Qt::green;
            else if (y >= yOffset + 150 && y < yOffset + 190) selectedColor = Qt::yellow;
            update(); return;
        }

        isLeftPressed = true;
        currentPartId++; // 每次按下左键即视为开始绘制一个新的独立区域
        handlePainting(event->pos(), false);
    }
    else if (event->button() == Qt::RightButton) {
        isRightPressed = true;
        handlePainting(event->pos(), true); // 右键为擦除
    }
}

void ArtisanWindow::mouseMoveEvent(QMouseEvent* event) {
    if (isLeftPressed) handlePainting(event->pos(), false);
    else if (isRightPressed) handlePainting(event->pos(), true);
}

void ArtisanWindow::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isLeftPressed = false;
        checkWin(); // 抬起左键时自动触发胜利判定
    }
    else if (event->button() == Qt::RightButton) isRightPressed = false;
}

// 核心绘画处理：利用 QPainterPath 的 contains 快速定位网格位置
void ArtisanWindow::handlePainting(QPoint pos, bool isEraser) {
    if (isChecking || levelPieces.isEmpty()) return;
    QColor colorToApply = isEraser ? Qt::white : selectedColor;
    for (int i = 0; i < levelPieces.size(); ++i) {
        if (levelPieces[i].path.contains(pos)) {
            if (levelPieces[i].color != colorToApply) {
                levelPieces[i].color = colorToApply;
                levelPieces[i].partId = isEraser ? -1 : currentPartId;
                if (paintSound && paintSound->isLoaded()) paintSound->play();
                update(); // 局部重绘
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
        QMessageBox::information(this, "艺术大师!", QString("第 %1 关 挑战成功!").arg(currentLevel));
        currentLevel++;
        loadLevel(currentLevel);
    }
    isChecking = false;
}

// 3. 绘图
void ArtisanWindow::paintEvent(QPaintEvent* event) {
    if (isChecking || levelPieces.isEmpty()) return;

    QStyleOption opt; opt.initFrom(this); // 支持样式表背景
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    painter.setRenderHint(QPainter::Antialiasing);

    // (A) 绘制基础网格
    for (const auto& piece : levelPieces) {
        painter.setBrush(piece.color);
        painter.setPen(QPen(QColor(60, 40, 20, 100), 1)); // 较细的基础边框
        painter.drawPath(piece.path);
    }

    // (B) 重点：动态边界高亮算法 (拓扑检测)
    // 逻辑：遍历所有格子，对比相邻格子的 partId。若 ID 不同，则说明此处应存在明显的分割线
    painter.setPen(QPen(QColor(212, 175, 55), 4)); // 使用 4 像素宽的金色画笔
    for (int i = 0; i < levelPieces.size(); ++i) {
        for (int j = 0; j < levelPieces.size(); ++j) {
            const auto& p1 = levelPieces[i], & p2 = levelPieces[j];
            if (p1.partId != p2.partId) {
                // 如果 p2 是 p1 的右侧邻居，且 partId 不同，在它们之间画竖线
                if (p2.col == p1.col + 1 && p2.row == p1.row)
                    painter.drawLine(p1.path.boundingRect().topRight(), p1.path.boundingRect().bottomRight());
                // 如果 p2 是 p1 的下方邻居，且 partId 不同，在它们之间画横线
                if (p2.row == p1.row + 1 && p2.col == p1.col)
                    painter.drawLine(p1.path.boundingRect().bottomLeft(), p1.path.boundingRect().bottomRight());
            }
        }
    }

    // (C) 绘制右侧 HUD 信息面板
    painter.setBrush(QColor(40, 30, 20, 220)); // 深褐半透明质感
    painter.setPen(Qt::NoPen);
    painter.drawRect(650, 0, 150, height());

    painter.setPen(QColor(230, 200, 150));
    QFont font = painter.font(); font.setBold(true); font.setPointSize(12);
    painter.setFont(font);
    painter.drawText(670, 50, QString("LEVEL: %1").arg(currentLevel));

    // (D) 绘制动态提示语 (调用 Handler 获取)
    if (levelStrategy) {
        painter.setPen(QColor(255, 255, 255));
        font.setPointSize(10); font.setBold(false); painter.setFont(font);
        painter.drawText(QRect(665, 200, 120, 100), Qt::AlignLeft | Qt::TextWordWrap, levelStrategy->getHint());
    }

    // (E) 绘制选色器 UI
    painter.setPen(QColor(200, 200, 200));
    font.setPointSize(11); painter.setFont(font);
    painter.drawText(670, 100, "Active Color:");

    painter.setBrush(selectedColor);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(695, 120, 40, 40); // 展示当前选中的色块

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