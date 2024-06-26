// ---------------------------------------------
// 抓包多线程
// ---------------------------------------------
#ifndef RECEIVEPACKETTHREAD_H
#define RECEIVEPACKETTHREAD_H
#define FRAME_DATA_LENGTH		1024
#define FRAME_DATA_HEAD_LENGTH	18
//#define MAX_PACKET_NUMBER

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

class ReceivePacketThread : public QThread
{
    Q_OBJECT

public:
    ReceivePacketThread();
    ReceivePacketThread(NetPcap *netcap, QString tmpFileName = "");
    void saveData(unsigned char *data, int len);
    void deleteData();
    void stop();
    void saveFile(QString strDataPath);

    int packetcount = 0;
    int packetlength[1024*1024];
    QString fileName="";
    QString dataPath="";

protected:
    void run();

private:
    volatile bool bStopped;
    NetPcap *pNetPcap;
    qint64 fileSize;
    QFile f;
    QElapsedTimer timer;
    qint64 startFileSize, startTime;
    //quint64 captureCount, captureTime;

signals:
    void signalFileStateChanged(QString msg);
    void signalFileCreated(QString name);
    void signalFileSizeChanged(QString size);
    void signalSpeedChanged(QString speed);
    void signalCaptureCount(QString count);
    void signalCaptureTime(QString count);
    void siganlPacketcapture(QString address);
};

#endif	// CAPTURETHREAD_H_
