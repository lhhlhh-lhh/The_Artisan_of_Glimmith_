#include "artisanwindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    // 确保 ArtisanWindow 构造函数里 loadLevel 的逻辑已经加上内存保护
    ArtisanWindow w;

    // 显示窗口
    w.show();

    // 进入 Qt 事件循环
    return a.exec();
}