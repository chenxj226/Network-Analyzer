#ifndef SENDSETTINGWINDOW_H
#define SENDSETTINGWINDOW_H

#include <QWidget>
#include <QtCore/QVariant>
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QWidget>
#include <QTimer>
#include <QComboBox>

class SendSettingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SendSettingWindow(QWidget *parent = nullptr);
    ~SendSettingWindow();

public:
    QLineEdit *editSpeed;
    QComboBox *comboxModel;
    QLineEdit *editNum;

private slots:
    void checkspeed();
    void setmodel();

};

#endif // SENDSETTINGWINDOW_H
