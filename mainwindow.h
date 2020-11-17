#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIntValidator>
#include <QDebug>
#include <QHostInfo>
#include <unistd.h>
#include <netdb.h>
#include <QTcpSocket>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QHostAddress>
#include "scanresult.h"
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void execute();
    void doScanFinished();
    void setOwnIP();
    void activateButton();
    void activateDeviceScanner();
    void reset();

private:
    Ui::MainWindow *ui;
    unsigned int m_countOpen;
    unsigned int m_countClosed;
    unsigned int m_countChecked;

    void checkPort( int portmin,int portmax, string ip);
    QString getOwnIP();
    /*
     * getService
     * Gets the service name from the service database
     */
    QString getService(const char *protocol, const int portNumber);
};

#endif // MAINWINDOW_H
