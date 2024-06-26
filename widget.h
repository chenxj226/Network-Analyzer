#ifndef WIDGET_H
#define WIDGET_H
#define TIMEMS          qPrintable(QTime::currentTime().toString("HH:mm:ss"))
#include <QVariant>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <qlogging.h>
#include <qmap.h>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QTimer>
#include <QGroupBox>
#include <QMessageBox>
#include <QCloseEvent>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTableWidget>
#include <QHeaderView>
#include  <direct.h>
#include "pcap.h"

#include "netpcap.h"
#include "recvpacketthread.h"
#include "recvviewwindow.h"
#include "sendpacketthread.h"
#include "sendsettingwindow.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    //QSettings *settings = nullptr;
    QTabWidget *tabConfigWidget;
    //QTabWidget *tabDisplayWidget;
    QWidget *widgetLog;
    QWidget *widgetNetworkState;
    QComboBox *comboxNetAdapter;
    QPushButton *btnRefreshNet;
    /*********send packet***********/
    QPushButton *btnSendStart;
    QPushButton *btnSendStop;
    QPushButton *btnFileImport;
    QPushButton *btnSendSetting;
    QLineEdit *editSrcAddress;
    QLineEdit *editDstAddress;
    QLineEdit *editProtocol;
    QTextEdit *txtSendLog;
//    QLineEdit *editSpeed;
    /*********capture packet***********/
    QPushButton *btnRecvStart;
    QPushButton *btnRecvStop;
    QPushButton *btnRecvView;
    QPushButton *btnRecvSave;
    QTextEdit *txtRecvLog;
    /****bottom*****/
    QLineEdit *editRecvSpeed;
    QLineEdit *editSendSpeed;

    QWidget *widgetSend;
    QWidget *widgetRecv;
    QGridLayout *mainLayout;


    QString strDataPath;
    uint32_t recordDataEnable;
    //    CaptureThread *captureThread;
    //    Sniffer *sniffer;
    int networkAdapterID;
    NetPcap * netpcap;
    ReceivePacketThread * recvthread;
    SendPacketThread * sendthread;
    RecvViewWindow *configWindow;
    SendSettingWindow *sendsetting;
    QTimer *mTimer;
    int timeId;

    int isrecord=0;
    int recvstartflag=1;
    int isdelete=0;

    void closeEvent(QCloseEvent *event);
    int isClearData();
    void initsetting();

    //QElapsedTimer testtime;

private slots:
    //net
    void btnRefreshClicked();
    //send
    void btnSendStartClicked();
    void btnSendStopClicked();
    void  btnSendNewClicked();
    void  btnSendSetClicked();
    //recivce
    void btnRecvStartClicked();
    void btnRecvStopClicked();
    void btnRecvViewClicked();
    void btnRecvSaveClicked();

};
#endif // WIDGET_H
