#include "artisanwindow.h"
#include "ui_artisanwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QCoreApplication>
#include <QUrl>
#include <QDebug>
#include <QFile>
#include <QStyleOption>
#include <QMap>

ArtisanWindow::ArtisanWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ArtisanWindow)
    , currentPartId(0) // 初始化笔画计数器
{
    ui->setupUi(this);

    // 调试资源文件是否存在
    qDebug() << "Resource file exists:" << QFile::exists(":/images/paper_bg.png");

    setFixedSize(800, 600);

    // --- 🎨 样式表配置 ---
    // 确保 WA_StyledBackground 开启，否则 QMainWindow 可能不响应样式表背景图
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
    setMouseTracking(true); // 开启鼠标追踪
}

ArtisanWindow::~ArtisanWindow() {
    delete ui;
}

// 1. 加载关卡逻辑：支持动态行列与坐标计算
void ArtisanWindow::loadLevel(int level) {
    levelPieces.clear();
    currentPartId = 0; // 重置当前关卡的笔画 ID

    int rows = 0, cols = 0;
    if (level == 1) {
        rows = 8; cols = 8; // 第一关 8x8
    }
    else {
        rows = 6; cols = 6; // 后续关卡默认 6x6
    }

    int size = 50; // 每个方格的边长
    // 居中计算：左侧游戏区域宽度约为 650
    int startX = (650 - cols * size) / 2;
    int startY = (height() - rows * size) / 2;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            GlassPiece p;
            p.row = r;
            p.col = c;
            p.path.addRect(startX + c * size, startY + r * size, size, size);
            p.color = Qt::white;
            p.partId = -1; // 初始化为未涂色状态
            levelPieces << p;
        }
    }
    update();
}

// 2. 鼠标按下：区分 UI 点击与绘图开启
void ArtisanWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // 如果点击位置在右侧工具栏（x > 650）
        if (event->pos().x() > 650) {
            // 这里可以添加点击色块切换 selectedColor 的逻辑
            if (QRect(680, 100, 50, 50).contains(event->pos())) {
                qDebug() << "Selected tool clicked";
            }
        }
        else {
            isLeftPressed = true;
            currentPartId++; // 开启一次新的“连续笔画”
            handlePainting(event->pos(), false);
        }
    }
    else if (event->button() == Qt::RightButton) {
        isRightPressed = true;
        handlePainting(event->pos(), true); // 右键默认为橡皮擦
    }
}

// 3. 鼠标移动：实现平滑的连续绘图
void ArtisanWindow::mouseMoveEvent(QMouseEvent* event) {
    if (isLeftPressed) {
        handlePainting(event->pos(), false);
    }
    else if (isRightPressed) {
        handlePainting(event->pos(), true);
    }
}

// 4. 鼠标释放：结束笔画并触发过关检测
void ArtisanWindow::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isLeftPressed = false;
        checkWin(); // 玩家松开鼠标时检测是否达成 4 个 4x4
    }
    else if (event->button() == Qt::RightButton) {
        isRightPressed = false;
    }
}

// 5. 核心涂色逻辑：记录 PartId 并优化音效播放
void ArtisanWindow::handlePainting(QPoint pos, bool isEraser) {
    QColor colorToApply = isEraser ? Qt::white : selectedColor;

    for (int i = 0; i < levelPieces.size(); ++i) {
        if (levelPieces[i].path.contains(pos)) {
            // 只有颜色改变时才处理，避免重复触发音效
            if (levelPieces[i].color != colorToApply) {
                levelPieces[i].color = colorToApply;
                // 如果是涂色，赋予当前笔画 ID；如果是擦除，重置为 -1
                levelPieces[i].partId = isEraser ? -1 : currentPartId;

                // 音效播放优化：不使用 stop() 以降低延迟
                if (paintSound && paintSound->isLoaded()) {
                    paintSound->play();
                }
                update();
            }
            break;
        }
    }
}

// 6. 过关判定逻辑：基于 PartId 的几何分析
void ArtisanWindow::checkWin() {
    if (currentLevel == 1) {
        // 使用 QMap 按 PartId 对格子进行分组
        QMap<int, QList<int>> groups;
        for (int i = 0; i < levelPieces.size(); ++i) {
            if (levelPieces[i].partId != -1) {
                groups[levelPieces[i].partId].append(i);
            }
        }

        int validSquareCount = 0;
        for (auto it = groups.begin(); it != groups.end(); ++it) {
            QList<int> indices = it.value();

            // 条件 A: 每一个连续的部分必须恰好由 16 个方格组成 (4x4=16)
            if (indices.size() != 16) continue;

            // 条件 B: 检查这 16 个格子的行列范围是否符合 4x4 形状
            int minR = 99, maxR = -1, minC = 99, maxC = -1;
            for (int idx : indices) {
                minR = qMin(minR, levelPieces[idx].row);
                maxR = qMax(maxR, levelPieces[idx].row);
                minC = qMin(minC, levelPieces[idx].col);
                maxC = qMax(maxC, levelPieces[idx].col);
            }

            // 如果高度和宽度均为 4，说明是一个完整的 4x4 正方形
            if ((maxR - minR + 1) == 4 && (maxC - minC + 1) == 4) {
                validSquareCount++;
            }
        }

        // 当场面上存在 4 个符合条件的 4x4 连续部分时过关
        if (validSquareCount == 4) {
            if (winSound && winSound->isLoaded()) winSound->play();
            QMessageBox::information(this, "Masterpiece!", "恭喜！你成功画出了 4 个 4x4 的连续区域！");
            currentLevel++;
            loadLevel(currentLevel);
        }
    }
}

// 7. 绘图事件：处理背景与游戏元素的层级
void ArtisanWindow::paintEvent(QPaintEvent* event) {
    // 绘制样式表背景（解决 QPainter 覆盖背景图的问题）
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制游戏格点
    for (const auto& piece : levelPieces) {
        painter.setBrush(piece.color);
        painter.setPen(QPen(QColor(60, 40, 20, 150), 2)); // 深啡色边框
        painter.drawPath(piece.path);
    }

    // 绘制右侧 UI 区域
    painter.setBrush(QColor(0, 0, 0, 40)); // 半透明遮罩
    painter.setPen(Qt::NoPen);
    painter.drawRect(650, 0, 150, height());

    // 绘制文字状态
    painter.setPen(QColor(80, 50, 30));
    QFont font = painter.font();
    font.setPixelSize(18);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(670, 50, QString("Level: %1").arg(currentLevel));

    // 绘制当前选中的颜色预览
    painter.setBrush(selectedColor);
    painter.setPen(QPen(QColor(60, 40, 20), 2));
    painter.drawRect(680, 100, 50, 50);
}

// 8. 加载音效：使用资源路径
void ArtisanWindow::loadSounds() {
    paintSound = std::make_unique<QSoundEffect>(this);
    winSound = std::make_unique<QSoundEffect>(this);

    // 使用 qrc 资源路径加载音效，确保跨平台兼容性
    paintSound->setSource(QUrl("qrc:/sounds/paint.wav"));
    winSound->setSource(QUrl("qrc:/sounds/win.wav"));

    paintSound->setVolume(0.5f);
    winSound->setVolume(0.7f);
}