#include "mainwindow.h"

#include <QApplication>
#include <Windows.h>

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    QApplication a(argc, argv);
    MainWindow w;   
    w.show();
    return a.exec();
}
