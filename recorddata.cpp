#include "recorddata.h"

RecordData::RecordData(QObject *parent)
{

}

void RecordData::start(QString dataPath)
{
    fileSize = 0;
    fileName = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".dat";
    QDir dir(dataPath);
    QString absFilePath = dir.absoluteFilePath(fileName);
    f.setFileName(absFilePath);
    f.open(QIODevice::ReadWrite | QIODevice::Append);
    emit signalFileStateChanged(tr("Data file name: ") + absFilePath + tr(" created."));
    emit signalFileCreated(absFilePath);
}

void RecordData::save(char *data, int len)
{
    qint64 ret = 0;
    ret = f.write(data, len);
    fileSize += ret;
    QLocale locale;
    QString str = locale.formattedDataSize(fileSize);
    emit signalFileSizeChanged(str);
}

void RecordData::stop()
{
    f.close();
    emit signalFileStateChanged(tr("Record data stop."));
    QLocale locale;
    QString str = locale.formattedDataSize(f.size());
    emit signalFileStateChanged(tr("File size: ")+str);
}
