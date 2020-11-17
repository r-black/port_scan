#include "scanresult.h"

ScanResult::ScanResult()
{

}

QString ScanResult::ip()
{
    return m_ip;
}

void ScanResult::setIp(const QString &ip)
{
    m_ip = ip;
}

int ScanResult::port()
{
    return m_port;
}

void ScanResult::setPort(const int &port)
{
    m_port = port;
}

bool ScanResult::found()
{
    return m_found;
}

void ScanResult::setFound(bool found)
{
    m_found = found;
}
