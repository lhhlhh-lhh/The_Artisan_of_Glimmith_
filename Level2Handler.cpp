#include "LevelHandler.h"
#include <algorithm>

class Level2Handler : public LevelHandler {
public:
    void loadLevel(QList<GlassPiece>& pieces, int screenWidth, int screenHeight) override {
        pieces.clear();
        int rows = 6, cols = 4, size = 50;
        int startX = (screenWidth - 150 - cols * size) / 2;
        int startY = (screenHeight - rows * size) / 2;

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                // 核心：实现 4x6 几何镂空
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
            if (p.partId == -1) return false; // 必须全部涂色
            groups[p.partId].append(p);
        }

        for (auto it = groups.constBegin(); it != groups.constEnd(); ++it) {
            const QList<GlassPiece>& group = it.value();
            int n = group.size();
            int minR = 9, maxR = -1, minC = 9, maxC = -1;
            for (const auto& p : group) {
                minR = std::min(minR, p.row); maxR = std::max(maxR, p.row);
                minC = std::min(minC, p.col); maxC = std::max(maxC, p.col);
            }
            int w = maxC - minC + 1, h = maxR - minR + 1;

            // 规则：1x2 Domino 或 3格 L-shape
            bool isDomino = (n == 2 && ((w == 1 && h == 2) || (w == 2 && h == 1)));
            bool isLShape = (n == 3 && w == 2 && h == 2);
            if (!isDomino && !isLShape) return false;
        }
        return true;
    }
};