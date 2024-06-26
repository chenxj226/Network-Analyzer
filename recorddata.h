#ifndef RECORDDATA_H
#define RECORDDATA_H

#include <QThread>
#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QLocale>
#include <QDir>

class RecordData : public QObject
{
    Q_OBJECT
public:
    explicit RecordData(QObject *parent = nullptr);
    void start(QString dataPath);
    void stop();

public slots:
    void save(char *data, int len);
signals:
    void signalFileStateChanged(QString msg);
    void signalFileCreated(QString name);
    void signalFileSizeChanged(QString size);
private:
    QString fileName;
    QFile f;
    qint64 fileSize;
};

#endif // RECORDDATA_H
