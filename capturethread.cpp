#include "capturethread.h"
//#include "listtreeview.h"
#include "sniffer.h"
#include "ethernetframe.h"

#ifdef WIN32
#pragma warning(disable:4996)
#endif

CaptureThread::CaptureThread()
{
    bStopped = false;
    sniffer  = NULL;
    fileSize = 0;
    captureCount = 0;
    captureTime = 0;
    startFileSize = 0;
    startTime = 0;
}

//CaptureThread::CaptureThread(Sniffer *pSniffer)
//{
//    bStopped = false;
//    sniffer  = pSniffer;
//    fileSize = 0;
//    dataPath = '/';
//    captureCount = 0;
//    captureTime = 0;
//    startFileSize = 0;
//    startTime = 0;
//}

CaptureThread::CaptureThread(Sniffer *pSniffer, QString path)
{
    bStopped = false;
    sniffer  = pSniffer;
    fileSize = 0;
    dataPath = path;
    captureCount = 0;
    captureTime = 0;
    startFileSize = 0;
    startTime = 0;
}

void CaptureThread::saveData(unsigned char *data, int len)
{
    qint64 endTime = 0, elapsedTime = 0;
    int ret;
    double speed;

    if (fileSize == 0) {
        timer.start();
    }
    //        rawByteData.setRawData((const char *)sniffer->pkt_data, sniffer->header->caplen);
    // qint64 ret = f.write((const char *)sniffer->pkt_data, sniffer->header->caplen);
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
            emit signalSpeedChanged(strSpeed);
        }
    }

    QLocale locale;
    QString str = locale.formattedDataSize(fileSize);
    emit signalFileSizeChanged(str);

//    emit signalCaptureCount(QString::number(captureCount));
//    emit signalCaptureTime(QString::number(endTime - captureTime));
    captureTime = endTime;
}

void CaptureThread::run()
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
        res = sniffer->captureOnce();
        if (res == 0) {
            continue;
        }
        unsigned char *ethernet_frame = (unsigned char *)sniffer->pkt_data;
        //int ethernet_frame_length = sniffer->header->caplen;

        unsigned char *frame_data_to_save = ethernet_frame + FRAME_DATA_HEAD_LENGTH;
//        int ret = parse_ethernet_frame(ethernet_frame, ethernet_frame_length);

//        switch (ret) {
//            case SELF_CHECK_START:
//                sniffer->sendOnce((unsigned char *)&self_check_success, sizeof(self_check_success));
//                break;
//            case HANDSHAKE_REQ:
//                sniffer->sendOnce((unsigned char *)&handshake_ack, sizeof(handshake_ack));
//                break;
//            case DATA_FRAME:
                saveData(frame_data_to_save, FRAME_DATA_LENGTH);
//                sniffer->sendOnce((unsigned char *)&data_frame_ack, sizeof(data_frame_ack));
//                break;

//            case FRAME_TOO_SHORT:
//            case SELF_CHECK_SUCCESS:
//            case SELF_CHECK_FAILURE:
//            case HANDSHAKE_ACK:
//            case DATA_FRAME_ACK:
//            case DATA_FRAME_RESEND:
//                break;

    }

}

//void CaptureThread::stop(const u_char**)
void CaptureThread::stop()
{
    bStopped = true;
    f.close();

    emit signalFileStateChanged(tr("Record data stop."));
    QLocale locale;
//    qint64 sz = f.size();
    QString str = locale.formattedDataSize(f.size());
    emit signalFileStateChanged(tr("File size: ")+str);
}
