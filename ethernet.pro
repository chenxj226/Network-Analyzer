QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    main.cpp \
    netpcap.cpp \
    recvpacketthread.cpp \
    sendpacketthread.cpp \
    recvviewwindow.cpp \
    sendsettingwindow.cpp\
    widget.cpp\
    qhexmonitor.cpp\


HEADERS += \
    ethernetframe.h \
    netpcap.h \
    recvpacketthread.h \
    sendpacketthread.h \
    recvviewwindow.h \
    sendsettingwindow.h \
    widget.h\
    qhexmonitor.h\


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += "D:/winpcap/WpdPack/Include"
LIBS += "-LD:/winpcap/WpdPack/Lib/x64" -lwpcap -lPacket

#FORMS += \
#    recvviewwindow.ui

#FORMS += \
#    sendsettingwindow.ui
