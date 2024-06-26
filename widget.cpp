#include "widget.h"
#include "QByteArray"
#include <QMap>
#include <QObject>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    //readSettings();

    setWindowTitle(tr("Ethernet Tools"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    /*********Net Adapter choose***********/
    QLabel *lblConnection = new QLabel(tr("Net Adapter:"));
    lblConnection->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    comboxNetAdapter = new QComboBox;
    btnRefreshNet = new QPushButton(tr("Refresh"));

    QGridLayout *layoutNet = new QGridLayout();
    layoutNet->addWidget(lblConnection,0,0,1,1);
    layoutNet->addWidget(comboxNetAdapter,0,2,1,10);
    layoutNet->addWidget(btnRefreshNet,0,14,1,1);

    /*********send packet***********/
    btnSendStart = new QPushButton(tr("Start"));
    btnSendStop = new QPushButton(tr("Stop"));
    btnFileImport = new QPushButton(tr("New"));
    btnSendSetting = new QPushButton(tr("Send Setting"));

    QLabel *lblSrcAddress = new QLabel(tr("Source Address:"));
    editSrcAddress = new QLineEdit;
    editSrcAddress->setReadOnly(true);
    editSrcAddress->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QLabel *lblDstAddress = new QLabel(tr("Destination Address:"));
    editDstAddress = new QLineEdit;
    editDstAddress->setReadOnly(true);
    editDstAddress->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QLabel *lblProtocol = new QLabel(tr("Protocol Type/Length:"));
    editProtocol = new QLineEdit;
    editProtocol->setReadOnly(true);
    editProtocol->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    txtSendLog = new QTextEdit;
    txtSendLog->setReadOnly(true);
    txtSendLog->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    txtSendLog->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    txtSendLog->setLineWrapMode(QTextEdit::NoWrap);

    QGridLayout *layoutSend = new QGridLayout();
    layoutSend->addWidget(btnSendStart,0,0,1,1);
    layoutSend->addWidget(btnSendStop,0,1,1,1);
    layoutSend->addWidget(btnFileImport,0,2,1,1);
    layoutSend->addWidget(btnSendSetting,0,3,1,2);

    layoutSend->addWidget(lblSrcAddress,2,0,1,1);
    layoutSend->addWidget(editSrcAddress,2,1,1,4);
    layoutSend->addWidget(lblDstAddress,2,5,1,1);
    layoutSend->addWidget(editDstAddress,2,6,1,4);
    layoutSend->addWidget(lblProtocol,2,10,1,1);
    layoutSend->addWidget(editProtocol,2,11,1,4);
    layoutSend->addWidget(txtSendLog,4,0,10,15);

    /*********capture packet***********/
    btnRecvStart = new QPushButton(tr("Start"));
    btnRecvStop = new QPushButton(tr("Stop"));
    btnRecvStop->setEnabled(0);
    btnRecvView = new QPushButton(tr("View"));
    btnRecvSave= new QPushButton(tr("Save"));

    txtRecvLog = new QTextEdit;
    txtRecvLog->setReadOnly(true);
    txtRecvLog->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    txtRecvLog->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    txtRecvLog->setLineWrapMode(QTextEdit::NoWrap);

    QGridLayout *layoutRecv = new QGridLayout();
    layoutRecv->addWidget(btnRecvStart,0,0,1,1);
    layoutRecv->addWidget(btnRecvStop,0,1,1,1);
    layoutRecv->addWidget(btnRecvView,0,2,1,1);
    layoutRecv->addWidget(btnRecvSave,0,3,1,1);
    layoutRecv->addWidget(txtRecvLog,2,0,11,15);

    QLabel *lblRecvSpeed = new QLabel(tr("Receive Speed:"));
    editRecvSpeed = new QLineEdit;
    editRecvSpeed->setReadOnly(true);
    QGridLayout *layoutBottom = new QGridLayout();

    layoutBottom->addWidget(lblRecvSpeed,0,0,1,1);
    layoutBottom->addWidget(editRecvSpeed,0,1,1,1);

    /*********main window***********/
    widgetSend = new QWidget();
    widgetSend->setLayout(layoutSend);
    widgetRecv = new QWidget();
    widgetRecv->setLayout(layoutRecv);

    tabConfigWidget = new QTabWidget;
    tabConfigWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tabConfigWidget->addTab(widgetSend, tr("Send"));
    tabConfigWidget->addTab(widgetRecv, tr("Capture"));

    mainLayout = new QGridLayout;
    mainLayout->addLayout(layoutNet,0,0,1,15);
    mainLayout->addWidget(tabConfigWidget,1,0,13,15);
    mainLayout->addLayout(layoutBottom,14,13,1,2);
    this->setLayout(mainLayout);

    initsetting();
}

Widget::~Widget()
{

}

void Widget::initsetting()
{
    netpcap = new NetPcap();
    sendsetting = new SendSettingWindow();
    sendsetting->setWindowModality(Qt::ApplicationModal);

    btnRefreshClicked();//find device after open
    recvthread = NULL;

    btnSendStop->setEnabled(0);
    btnRecvSave->setEnabled(0);
    btnRecvView->setEnabled(0);
    connect(btnRefreshNet, SIGNAL(clicked()), this, SLOT(btnRefreshClicked()));
    connect(btnSendStart, SIGNAL(clicked()), this, SLOT(btnSendStartClicked()));
    connect(btnSendStop, SIGNAL(clicked()), this, SLOT(btnSendStopClicked()));
    connect(btnSendSetting, SIGNAL(clicked()), this, SLOT(btnSendSetClicked()));
    connect(btnRecvStart, SIGNAL(clicked()), this, SLOT(btnRecvStartClicked()));
    connect(btnRecvStop, SIGNAL(clicked()), this, SLOT(btnRecvStopClicked()));
    connect(btnRecvView, SIGNAL(clicked()), this, SLOT(btnRecvViewClicked()));
    connect(btnRecvSave,SIGNAL(clicked()), this, SLOT(btnRecvSaveClicked()));
}

void Widget::btnRefreshClicked()
{
    QVector<DEVInfo> devInfo(netpcap->findAllDev());//find device
    comboxNetAdapter->clear();
    for(int i = 0; i < devInfo.length(); ++i){
        QPixmap icon  = style()->standardPixmap(QStyle::SP_DriveNetIcon);
        comboxNetAdapter->addItem(icon,devInfo.at(i).name);
    }
}

void Widget::btnSendStartClicked()
{

    QString devStr = comboxNetAdapter->currentText();
    QByteArray devByteArray = devStr.toUtf8();
    const char *devName = devByteArray.data();
    bool bOpenSucceed = netpcap->openLiveDev(devName);//open device

    txtSendLog->append(QString(tr("[%1]").arg(TIMEMS)) + tr("start sending"));
    sendthread = new SendPacketThread(netpcap);
    sendthread->SendSpeed = sendsetting->editSpeed->text().toInt();
    sendthread->SendType = sendsetting->comboxModel->currentIndex();
    sendthread->SendNum = sendsetting->editNum->text().toInt();

    if (bOpenSucceed) {//opened device successfully
        sendthread->start();
    }
    else{
        txtSendLog->append(QString(tr("[%1]").arg(TIMEMS)) + tr("fail to open device"));
    }

    connect(netpcap, SIGNAL(signalSendcomplete()),this,SLOT(btnSendStopClicked()));//Connect 'Send Complete' and 'Stop' for the mode of 'brust'
    btnSendStart->setEnabled(0);
    btnSendStop->setEnabled(1);
    btnSendSetting->setEnabled(0);
    btnFileImport->setEnabled(0);
    widgetRecv->setEnabled(0);
    comboxNetAdapter->setEnabled(0);
}

void Widget::btnSendStopClicked()
{
    sendthread->stop();
    txtSendLog->append(QString(tr("[%1]").arg(TIMEMS)) + tr("stop sending"));
    txtSendLog->append(QString(tr("------total send ") + QString::number(netpcap->sendcount) + tr(" packet-------")));

    btnSendStart->setEnabled(1);
    btnSendStop->setEnabled(0);
    btnSendSetting->setEnabled(1);
    btnFileImport->setEnabled(1);
    widgetRecv->setEnabled(1);
    comboxNetAdapter->setEnabled(1);
    disconnect(netpcap, SIGNAL(signalSendcomplete()),this,SLOT(btnSendStopClicked()));
}

void Widget::btnSendNewClicked()
{

}

void Widget::btnSendSetClicked()
{
    sendsetting->show();
}

void Widget::btnRecvStartClicked()
{
    if (isrecord != 0)
    {
        recvstartflag = isClearData();//if there is data, ask whether to save it
    }
    if (recvstartflag!=0)
    {
        if (isdelete != 0)
        {
            recvthread->deleteData();//delete data
        }
        char   buffer[MAX_PATH];
        _getcwd(buffer, MAX_PATH);
        QString filename_vertices = QString::fromUtf8(buffer);

        QString devStr = comboxNetAdapter->currentText();
        QByteArray devByteArray = devStr.toUtf8();
        const char *devName = devByteArray.data();
        bool bOpenSucceed = netpcap->openLiveDev(devName);//open device
        if (recvthread != NULL) {
            delete recvthread;
        }
        recvthread = new ReceivePacketThread(netpcap, filename_vertices);

        connect(recvthread, SIGNAL(signalSpeedChanged(QString)), editRecvSpeed, SLOT(setText(QString)));
        if (bOpenSucceed) {
            recvthread->start();
        }
        txtRecvLog->append(QString(tr("[%1]").arg(TIMEMS)) + tr("start receiving"));

        btnRecvStop->setEnabled(1);
        btnRecvStart->setEnabled(0);
        btnRecvSave->setEnabled(0);
        btnRecvView->setEnabled(0);
        btnRefreshNet->setEnabled(0);
        comboxNetAdapter->setEnabled(0);
        widgetSend->setEnabled(0);
    }
}

void Widget::btnRecvStopClicked()
{
    recvthread->stop();

    txtRecvLog->append(QString(tr("[%1]").arg(TIMEMS)) + tr("stop receiving"));
    Sleep(200);
    txtRecvLog->append(tr("------total receive ") + QString::number(recvthread->packetcount) + tr(" packet-------"));
    if (recvthread->packetcount != 0){
        isrecord = 1;//if there is data, set 1
        btnRecvSave->setEnabled(1);
        btnRecvView->setEnabled(1);
    }
    else{
        isrecord = 0;
    }
    isdelete = 1;
    disconnect(recvthread, SIGNAL(signalSpeedChanged(QString)), editRecvSpeed, SLOT(setText(QString)));
    btnRecvStop->setEnabled(0);
    btnRecvStart->setEnabled(1);
    btnRefreshNet->setEnabled(1);
    comboxNetAdapter->setEnabled(1);
    widgetSend->setEnabled(1);
}

void Widget::btnRecvViewClicked()
{
    configWindow = new RecvViewWindow(nullptr,recvthread);
    configWindow->setWindowModality(Qt::ApplicationModal);
    configWindow->show();
}

void Widget::btnRecvSaveClicked()
{
    strDataPath = QFileDialog::getExistingDirectory(this, tr("Select Data Saving Path..."), strDataPath);
    if (!strDataPath.isEmpty())
    {
        recvthread->saveFile(strDataPath);
        txtRecvLog->append(QString(tr("[%1]").arg(TIMEMS)) + tr("Data saved in path:") + strDataPath +recvthread->fileName);
    }
}

void Widget::closeEvent(QCloseEvent *event)
{
    int button;
    button = QMessageBox::question(this, tr("Information"),
                                   QString(tr("Do you really want to Exit?")),
                                   QMessageBox::Yes | QMessageBox::No);
    if (button == QMessageBox::No) {
        event->ignore();
    }
    else if (button == QMessageBox::Yes) {
        if (isdelete != 0){recvthread->deleteData();}
        event->accept();
    }
}

int Widget::isClearData()
{
    int button;
    button = QMessageBox::question(this, tr("Information"),
                                   QString(tr("Do you want to clear data?")),
                                   QMessageBox::Yes |QMessageBox::Save | QMessageBox::No);
    if (button == QMessageBox::No) {
        return 0;
    }
    else if (button == QMessageBox::Yes) {
        //        getSelections();
        //        saveSettings();
        return 1;
    }
    else if (button == QMessageBox::Save) {
        btnRecvSaveClicked();
        return 0;
    }
    return 0;
}
