// ---------------------------------------------
// 发包多线程
// ---------------------------------------------
#ifndef SENDPACKETTHREAD_H
#define SENDPACKETTHREAD_H

#include <QThread>
#include <QLocale>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QDebug>
#include "netpcap.h"
#include "pcap.h"

class NetPcap;

class SendPacketThread : public QThread
{
    Q_OBJECT

public:
    //
    SendPacketThread();
    SendPacketThread(NetPcap *netcap);
    ~SendPacketThread();
    void stop();

    int count = 0;
    int SendType = 0;
    int SendSpeed = 1;
    int SendNum = 1000;

protected:
    void run();

private:
    NetPcap *pNetPcap;

    const u_char send_data[1024]={0};
    qint64 len;
    QTimer *mTimer;
    //int count = 0;
    bool bstop = false;

signals:

public slots:
//    void send();
};

#endif // SENDPACKETTHREAD_H
