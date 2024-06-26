#ifndef RECVVIEWWINDOW_H
#define RECVVIEWWINDOW_H

#include <QtCore/QVariant>
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>
#include <QComboBox>
#include "qhexmonitor.h"
#include "netpcap.h"
#include "recvpacketthread.h"

class RecvViewWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RecvViewWindow(QWidget *parent = nullptr, ReceivePacketThread * recvthread = nullptr);
    ~RecvViewWindow();

public:
    void viewData(ReceivePacketThread * recvthread);
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_4;
    //QGridLayout *verticalLayout_4;
    QLabel *label;
    QPushButton *pushButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QComboBox * dataList;
    QByteArray datas;
    int count = 0;
    int *lens = NULL;
    int tempnum=0;

private slots:
    void btnShowDataclicked();

private:
    QHexMonitor *hexEdit;
};

#endif // RECVVIEWWINDOW_H
