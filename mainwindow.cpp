#include "mainwindow.h"
#include "ui_mainwindow.h"

ScanResult asyncScan(QString& ip, int port)
{
    QTcpSocket *socket=new QTcpSocket;
    socket->connectToHost(ip, port);
    socket->waitForConnected(2000);
    bool connected=false;
    if(socket->state()==QTcpSocket::ConnectedState)
    {
        connected=true;
    }
    socket->close();
    socket->deleteLater();
    ScanResult scanResult;
    scanResult.setIp(ip);
    scanResult.setPort(port);
    scanResult.setFound(connected);
    return scanResult;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButtonScan->setEnabled(false);
    connect(ui->pushButtonScan, SIGNAL(released()), this, SLOT(execute()));
    connect(ui->checkBoxLHost, SIGNAL(clicked(bool)), this, SLOT(setOwnIP()));
    connect(ui->checkBoxDScan, SIGNAL(clicked(bool)), this, SLOT(activateDeviceScanner()));
    connect(ui->linePortStart, SIGNAL(textChanged(QString)), this, SLOT(activateButton()));
    connect(ui->linePortEnd, SIGNAL(textChanged(QString)), this, SLOT(activateButton()));
    connect(ui->lineIPStart, SIGNAL(textChanged(QString)), this, SLOT(activateButton()));
    connect(ui->pushButtonReset, SIGNAL(clicked(bool)), this, SLOT(reset()));
    ui->lineIPStart->setInputMask("000.000.000.000;_");
    ui->lineIPEnd->setInputMask("000.000.000.000;_");
    ui->linePortStart->setValidator( new QIntValidator(0, 65535, this) );
    ui->linePortEnd->setValidator( new QIntValidator(0, 65535, this) );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::execute()
{
    quint32 ip1=QHostAddress(ui->lineIPStart->text()).toIPv4Address();
    m_countOpen = 0;
    m_countClosed = 0;
    m_countChecked = 0;
    int dif = 0;
    if(ui->checkBoxDScan->isChecked()) {
        quint32 ip2 = QHostAddress(ui->lineIPEnd->text()).toIPv4Address();
        dif = ip2 - ip1;
        if(dif < 0 || dif > 300) {
            ui->textEditResult->append(tr("The IP2 should be larger than IP1, the total number of ip addresses should be smaller than 300！"));
            return;
        }
        for(quint32 ip = ip1; ip <= ip2; ip++) {
            qDebug() << QHostAddress(ip).toString();
            QFutureWatcher<ScanResult> *watcher = new QFutureWatcher<ScanResult>;
            connect(watcher, SIGNAL(finished()), this, SLOT(doScanFinished()));
            QFuture<ScanResult> future = QtConcurrent::run(asyncScan, QHostAddress(ip).toString(), ui->linePortStart->text().toInt());
            watcher->setFuture(future);
        }
    } else {
        int portmin = ui->linePortStart->text().toInt();
        int portmax = ui->linePortEnd->text().toInt();
        dif = portmax - portmin;
        if (dif <= 0) {
            ui->textEditResult->append(tr("The Port2 should be larger than Port1！"));
            return;
        }
        for(int port = portmin; port <= portmax; port++) {
            qDebug() << port;
            QFutureWatcher<ScanResult> *watcher = new QFutureWatcher<ScanResult>;
            connect(watcher, SIGNAL(finished()), this, SLOT(doScanFinished()));
            QFuture<ScanResult> future;
            try {
                future = QtConcurrent::run(asyncScan, QHostAddress(ip1).toString(), port);
            } catch (...) {
                qDebug() << "Concurrent Error!";
            }
            watcher->setFuture(future);
        }
    }

    ui->progressBar->setMaximum(dif);
    ui->pushButtonScan->setEnabled(false);
}

void MainWindow::doScanFinished()
{
    QFutureWatcher<ScanResult> *watcher=static_cast<QFutureWatcher<ScanResult>*>(sender());
    ScanResult result=watcher->result();
    QString resultMsg = "";
    if (ui->checkBoxDScan->isChecked()) {
        resultMsg = "IP " + result.ip();
    } else {
        resultMsg = "Service Name: " + getService("tcp", result.port()) + ". Port " + QString::number(result.port());
    }
    if(result.found()) {
        ui->textEditResult->setTextColor(Qt::green);
        resultMsg += " is open";
        ui->textEditResult->append(resultMsg);
        m_countOpen++;
        QApplication::beep();
    } else {
        ui->textEditResult->setTextColor(Qt::red);
        resultMsg += " is closed";
        ui->textEditResult->append(resultMsg);
        m_countClosed++;
    }
    m_countChecked++;
    ui->progressBar->setValue(m_countChecked);

    QPalette *pal1 = new QPalette();
    pal1->setColor(QPalette::WindowText, Qt::green);
    ui->labelCountOpen->setPalette(*pal1);
    ui->labelCountOpen->setText(QString::number(m_countOpen));

    QPalette *pal2 = new QPalette();
    pal2->setColor(QPalette::WindowText, Qt::red);
    ui->labelCountClosed->setPalette(*pal2);
    ui->labelCountClosed->setText(QString::number(m_countClosed));

    QApplication::processEvents();
    watcher->deleteLater();
    activateButton();
}

void MainWindow::setOwnIP()
{
    if (ui->checkBoxLHost->isChecked())
        ui->lineIPStart->setText(getOwnIP());
    if (!(ui->checkBoxLHost->isChecked()))
        ui->lineIPStart->setText("");
}

QString MainWindow::getOwnIP()
{
    QString ip = "127.0.0.1";
//    QHostInfo info = QHostInfo::fromName( QHostInfo::localHostName() );
//    QList<QHostAddress> l= info.addresses();
//    for(int i=0; i<l.count(); i++) {
//        ip += l[i].toString();
//    }
    return ip;
}

void MainWindow::activateButton()
{
    if (ui->linePortStart->text() != "" && ui->lineIPStart->text()!= "") {
        ui->pushButtonScan->setEnabled(true);
    }
    else {
        ui->pushButtonScan->setEnabled(false);
    }
}

void MainWindow::activateDeviceScanner()
{
    if (ui->checkBoxDScan->isChecked()) {
        ui->lineIPEnd->setEnabled(true);
        ui->linePortEnd->setEnabled(false);
        ui->linePortEnd->setText("");
    }
    if (!ui->checkBoxDScan->isChecked()) {
        ui->lineIPEnd->setEnabled(false);
        ui->lineIPEnd->setText("");
        ui->linePortEnd->setEnabled(true);
    }
}

void MainWindow::reset()
{
    ui->textEditResult->clear();
    ui->linePortEnd->clear();
    ui->linePortStart->clear();
    ui->progressBar->setValue(0);
    ui->labelCountOpen->setText("");
    ui->labelCountClosed->setText("");
}

QString MainWindow::getService(const char *protocol, const int portNumber)
{

    QString serviceName = "NA";
    struct servent* serviceInfo;
    serviceInfo = getservbyport(htons(portNumber), protocol );
    if(serviceInfo)
        serviceName =QString(serviceInfo->s_name);
    return serviceName;
}
