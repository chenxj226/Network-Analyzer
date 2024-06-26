// ---------------------------------------------
// 获取本机Mac地址多线程
// ---------------------------------------------
#ifndef GETMACTHREAD_H
#define GETMACTHREAD_H

#include <QThread>
#include <QTimer>
#include "pcap.h"
#include "tcpipcommon.h"

class GetMacThread : public QThread
{
    Q_OBJECT

public:
    GetMacThread():handle(NULL){}
    GetMacThread(const char *devname,const char *ipAddr);
    GetMacThread(pcap_t *handle,const char *ipAddr);
    ~GetMacThread();
    QString getSelfMac();
    void run();

private:
    pcap_t *handle;
    char hostIp[16];
    QTimer * timer;
signals:
    void getSelfMacFinishedSig(QString mac);    
};

#endif // GETMACTHREAD_H
