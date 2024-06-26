#include "recvviewwindow.h"
//#include "ui_recvviewwindow.h"

RecvViewWindow::RecvViewWindow(QWidget *parent, ReceivePacketThread * recvthread)
    : QWidget(parent)
{
    setWindowTitle(tr("Ethernet Frame"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    this->resize(1000, 1000);

    label = new QLabel(tr("data length:0"));
    label->setAlignment(Qt::AlignCenter);

    dataList = new QComboBox;

    pushButton = new QPushButton(tr("show"));

    verticalLayout_4 = new QVBoxLayout;
    verticalLayout_4->setSpacing(6);
    verticalLayout_4->setContentsMargins(11, 11, 11, 11);
    verticalLayout_4->addWidget(dataList);
    verticalLayout_4->addWidget(pushButton);
    verticalLayout_4->addWidget(label);

    hexEdit=new QHexMonitor(this);
    verticalLayout_4->addWidget(hexEdit);
    this->setLayout(verticalLayout_4);

    connect(pushButton,SIGNAL(clicked()), this,SLOT(btnShowDataclicked()));

    count = recvthread->packetcount;
    lens = recvthread->packetlength;
    viewData(recvthread);
}

RecvViewWindow::~RecvViewWindow()
{

}

void RecvViewWindow::btnShowDataclicked()
{
    QByteArray datashow;
    tempnum = 0;
    int ind = dataList->currentIndex();
    for (int i = 0; i < ind; i++)
    {
        tempnum = tempnum + lens[i];
    }
    for(int k=0;k<lens[ind];k++)
    {
        datashow[k] = datas[tempnum+k];
    }
    hexEdit->append(datashow);
    this->label->setText(tr("data length:") + QString::number(hexEdit->getDateSize()));
}

void RecvViewWindow::viewData(ReceivePacketThread * recvthread)
{
    QDir dir(recvthread->dataPath);
    QString absFilePath = dir.absoluteFilePath(recvthread->fileName);
    QFile f;
    f.setFileName(absFilePath);
    f.open(QIODevice::ReadOnly);
    datas = f.readAll();
    f.close();
    //QByteArray * dataptr= &datas;
    QByteArray dataInfo;
    tempnum = 0;
    for (int i=0; i<count; i++){
        if (i!=0){
            tempnum = tempnum + lens[i-1];
        }
        for(int k=0;k<12;k++){
            dataInfo[k] = datas[tempnum+k];
        }
        QString item = QString::number(i+1) + tr(".address:") + dataInfo.toHex();
        dataList->addItem(item);
    }
}
