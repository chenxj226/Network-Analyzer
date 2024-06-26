#ifndef NETPCAP_H
#define NETPCAP_H

#include <QString>
#include <QVector>
#include <QObject>
#include <QPair>
#include <QMetaType>
#include <QQueue>
#include <QTimer>
#include "pcap.h"
//#include "tcpipcommon.h"
//#include "sendpacketthread.h"

typedef struct DEVInfo{
    QString name;
    QString description;
    QString familyName;         //协议族，
    QString address;            //主机ip
    QString netmask;            //子网掩码
}DEVInfo;


class NetPcap : public QObject
{
    Q_OBJECT
public:
    NetPcap();
    ~NetPcap();

public:
    // 扫描本机所有的适配器，并获取每个适配器的信息
    QVector<DEVInfo> findAllDev();
    // 打开一个适配器
    bool openLiveDev(const char *);
    bool closeLiveDev();
    // 获取当前打开的适配器描述符
    int captureOnce();
    int sendOnce(const u_char *send_data,qint64 len);

public:
    struct	pcap_pkthdr *header;
    const u_char		*pkt_data;
    pcap_t * handle; //设备指针
    bool pcapStatus;
//    HostInfo hostInfo;
    pcap_if_t *alldevs;// 网络设备列表信息链表指针
    pcap_dumper_t	*pDumpFile;						// 要保存到的文件指针
    //send
    int sendcount = 0;
    int mseccount = 0;
    const u_char send_data[1024]={0};
    int send_len =1024;
    int type =1;
    int num =1;
    int sendspeed=0;

//    QTimer *getDataFromQQueueTimer;
//    QTimer *getDataFromFilterBufferTimer;
//    QMap< QString,SendPacketThread * > *sendThreadAdd;
//    QQueue< QPair<QString,QString> > *hostInfoBuffer;
//    QQueue< QString > *filterDataBuffer;
//    FilterThread *filterThread;
public slots:
    void sendtrigger();

signals:
    void signalSendcomplete();
};

#endif // NETPCAP_H
