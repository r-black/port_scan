#include "mainwindow.h"
#include <QApplication>
#include <QThread>
#include <QThreadPool>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QThreadPool::globalInstance()->setMaxThreadCount(100);
    MainWindow w;
    w.setWindowTitle(QObject::tr("TCP Port Scanner"));
    w.show();

    return a.exec();
}
