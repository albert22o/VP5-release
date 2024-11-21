#include "mainwindow.h"

#include "dialogWindows/menudialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MenuDialog menu;
    menu.show();

    return a.exec();
}
