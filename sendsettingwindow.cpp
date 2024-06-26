#include "sendsettingwindow.h"

#include <QDebug>

SendSettingWindow::SendSettingWindow(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle(tr("Send Setting"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    this->resize(100, 100);


    QLabel *lblSendRate = new QLabel(tr("Send Packet Rate:"));
    editSpeed = new QLineEdit;
    editSpeed->setValidator(new QIntValidator(1,100000, editSpeed));
    editSpeed->setText("1");
    QLabel *lblSpeedUnit = new QLabel(tr("/s"));

    QLabel *lblSendModel = new QLabel(tr("Send Model:"));
    comboxModel = new QComboBox;
    comboxModel->addItem(tr("continue"));
    comboxModel->addItem(tr("burst"));

    editNum = new QLineEdit;
    editNum->setValidator(new QIntValidator(1,999999, editNum));
    editNum->setReadOnly(true);


    QGridLayout *layoutSendSetting = new QGridLayout;
    layoutSendSetting->addWidget(lblSendRate,0,1,1,1);
    layoutSendSetting->addWidget(editSpeed,0,2,1,2);
    layoutSendSetting->addWidget(lblSpeedUnit,0,4,1,1);
    layoutSendSetting->addWidget(lblSendModel,4,1,1,1);
    layoutSendSetting->addWidget(comboxModel,4,2,1,2);
    layoutSendSetting->addWidget(editNum,5,2,1,2);

    this->setLayout(layoutSendSetting);

    //    groupboxDeviceState = new QGroupBox(tr("Device State"));
    //    groupboxDeviceState->setLayout(layoutDeviceState);
    connect(editSpeed, SIGNAL(textChanged()),this,SLOT(checkspeed()));
    connect(comboxModel, SIGNAL(currentIndexChanged(int)),this,SLOT(setmodel()));

}

SendSettingWindow::~SendSettingWindow()
{

}

void SendSettingWindow::checkspeed()
{
    if (editSpeed->text().isEmpty()){
        editSpeed->setText("1");
    }
}

void SendSettingWindow::setmodel()
{
    //qDebug(QString::number(comboxModel->currentIndex()));
    if(comboxModel->currentIndex() == 0)
    {
        editNum->setReadOnly(true);
    }
    else{
        editNum->setReadOnly(false);
    }
}
