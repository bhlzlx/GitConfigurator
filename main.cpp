#include <QApplication>
#include "configure_window.h"

int main( int argc, char** argv) {
    QApplication app( argc, argv );
    ConfigureWindow* confWin = new ConfigureWindow();
    confWin->show();
    auto rst = app.exec();
    return rst;
}