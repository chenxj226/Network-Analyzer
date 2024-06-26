#include "sendpacketthread.h"
#include <QDebug>


SendPacketThread::SendPacketThread()
{

}
SendPacketThread::SendPacketThread(NetPcap *netcap)
{
    pNetPcap=netcap;
}

SendPacketThread::~SendPacketThread()
{
}

void SendPacketThread::run()
{
        mTimer = new QTimer();
        connect(mTimer,SIGNAL(timeout()),pNetPcap,SLOT(sendtrigger()));

        pNetPcap->type=SendType;
        pNetPcap->sendspeed = SendSpeed;
        pNetPcap->num=SendNum;

        mTimer->start(10);//10ms触发一次
        pNetPcap->sendcount = 0;
        pNetPcap->mseccount = 0;
        exec();

    //计算程序运行时间方法
//    QTime timedebuge;//声明一个时钟对象
//    timedebuge.start();//开始计时

//    pNetPcap->sendcount = 0;
//    while(bstop!=true){
//        pcap_sendpacket(pNetPcap->handle, pNetPcap->send_data, 1024);
//        pNetPcap->sendcount++;
//    }
//        qDebug()<<timedebuge.elapsed()/1000.0<<"s";//输出计时

}

void SendPacketThread::stop()
{
    bstop = true;
    disconnect(mTimer,SIGNAL(timeout()),pNetPcap,SLOT(sendtrigger()));
    exit();
}
