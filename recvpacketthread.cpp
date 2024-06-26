#include "recvpacketthread.h"


#ifdef WIN32
#pragma warning(disable:4996)
#endif

ReceivePacketThread::ReceivePacketThread()
{
    bStopped = false;
    pNetPcap = NULL;
    fileSize = 0;
    dataPath = "/";
    //captureCount = 0;
    //captureTime = 0;
    startFileSize = 0;
    startTime = 0;
}

//
ReceivePacketThread::ReceivePacketThread(NetPcap *netpcap, QString path)
{
    bStopped = false;
    pNetPcap = netpcap;
    fileSize = 0;
    dataPath = path;
    //captureCount = 0;
    //captureTime = 0;
    startFileSize = 0;
    startTime = 0;
}

void ReceivePacketThread::saveData(unsigned char *data, int len)
{

    qint64 endTime = 0, elapsedTime = 0;
    int ret;
    double speed;

    if (fileSize == 0) {
        timer.start();
    }
    ret = f.write((const char *)data, len);
    fileSize += ret;

    endTime = timer.elapsed();
    if ((endTime > 0)  && (endTime%500 == 0))
    {
        elapsedTime = endTime - startTime;
        if (elapsedTime > 0)
        {
            speed = (fileSize - startFileSize)*1.0 / elapsedTime * 1000/1024/1024;
            startTime = endTime;
            startFileSize = fileSize;
            QString strSpeed = QString::number(speed, 'g', 2);
            strSpeed = strSpeed + "M/s";
            emit signalSpeedChanged(strSpeed);
        }
    }
    QLocale locale;
    QString str = locale.formattedDataSize(fileSize);
    emit signalFileSizeChanged(str);
    //captureTime = endTime;
}

void ReceivePacketThread::deleteData()
{
    QDir dir(dataPath);
    QString absFilePath = dir.absoluteFilePath(fileName);
    f.setFileName(absFilePath);
    f.remove();
}

void ReceivePacketThread::saveFile(QString strDataPath)
{
    QDir dir(dataPath);
    QString absFilePath = dir.absoluteFilePath(fileName);
    QDir dir1(strDataPath);
    QString copyDataPath = dir1.absoluteFilePath(fileName);
    QFile currenfile(absFilePath);
    currenfile.copy(copyDataPath);
}

void ReceivePacketThread::run()
{
    int res;
    fileName = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".dat";
    QDir dir(dataPath);
    QString absFilePath = dir.absoluteFilePath(fileName);
    f.setFileName(absFilePath);
    f.open(QIODevice::ReadWrite | QIODevice::Append);
    emit signalFileStateChanged(tr("Data file name: ") + absFilePath + tr(" created."));
    emit signalFileCreated(absFilePath);
    while (bStopped != true)
    {
        res = pNetPcap->captureOnce();
        if (res == 0) {
            continue;
        }
        unsigned char *ethernet_frame = (unsigned char *)pNetPcap->pkt_data;
        int ethernet_frame_length_temp = pNetPcap->header->caplen;
        packetlength[packetcount] = ethernet_frame_length_temp;
        saveData(ethernet_frame, ethernet_frame_length_temp);
        packetcount++;
    }
}

//void CaptureThread::stop(const u_char**)
void ReceivePacketThread::stop()
{
    bStopped = true;
    f.close();
    emit signalFileStateChanged(tr("Record data stop."));
    QLocale locale;
//    qint64 sz = f.size();
    QString str = locale.formattedDataSize(f.size());
    emit signalFileStateChanged(tr("File size: ")+str);
}

