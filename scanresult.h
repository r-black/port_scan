#ifndef SCANRESULT_H
#define SCANRESULT_H

#include <QString>

class ScanResult
{
public:
    ScanResult();

    QString ip();
    void setIp(const QString &ip);

    int port();
    void setPort(const int &port);

    bool found();
    void setFound(bool found);

private:
    QString m_ip;
    int m_port;
    bool m_found;
};

#endif // SCANRESULT_H
