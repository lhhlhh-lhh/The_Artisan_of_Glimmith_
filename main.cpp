#include "artisanwindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    ArtisanWindow w;
    w.show();
    return a.exec();
}