#include "qhexmonitor.h"
#include <QDebug>
#include <QClipboard>
#include <QKeyEvent>
#include <QPainter>
#include <QScrollBar>
#include <QApplication>
#include <QClipboard>
#include <QInputDialog>
#include <QFontDialog>
QHexMonitor::QHexMonitor(QWidget *parent) : QAbstractScrollArea(parent)
{
    setFont(QFont("Courier", 10));
    setAcceptDrops(true);
    //setDarkStyle();
    createActions();
    setLightStyle();
}

void QHexMonitor::setColumn(int count)
{
    if(count<MIN_COL_COUNT)_colCount=MIN_COL_COUNT;
    else if(count>MAX_COL_COUNT)_colCount=MAX_COL_COUNT;
    else _colCount=count;
    adjustColumn();
    viewport()->update();
}
void QHexMonitor::createActions()
{
    //创建菜单、菜单项
    pop_menu = new QMenu(this);

    actionCopyHex   = pop_menu->addAction("copy Hex");
    actionCopyStr   = pop_menu->addAction("copy string");
    actionSelectAll = pop_menu->addAction("all select");
    actionClear     = pop_menu->addAction("clear");
    actionAutoScroll= pop_menu->addAction("auto down");
    actionSetColumn = pop_menu->addAction("set With");

    actionAutoScroll->setCheckable(true);

    //连接信号与槽
    connect(actionCopyHex, SIGNAL(triggered(bool)), this, SLOT(copyHex()));
    connect(actionCopyStr, SIGNAL(triggered(bool)), this, SLOT(copyStr()));
    connect(actionSelectAll, SIGNAL(triggered(bool)), this, SLOT(selectAll()));
    connect(actionClear, SIGNAL(triggered(bool)), this, SLOT(clearAll()));
    connect(actionAutoScroll, SIGNAL(triggered(bool)), this, SLOT(setAutoScroll(bool)));
    connect(actionSetColumn, SIGNAL(triggered(bool)), this, SLOT(setColumn()));
}
void QHexMonitor::append(QByteArray ba)
{
    _date.clear();
    _date.append(ba);
    viewport()->update();
}
void QHexMonitor::setFont(const QFont &font)
{
    QFont theFont(font);
    theFont.setStyleHint(QFont::Monospace);
    QWidget::setFont(theFont);
    QFontMetrics metrics = fontMetrics();
    _pxCharWidth = metrics.width(QLatin1Char('2'));
    _pxCharHeight = metrics.height();
    _markTopLeft.setX(_pxCharWidth*10);
    _markTopLeft.setY(_pxCharHeight);
    _topLeft=_markTopLeft-QPoint(0,4);
    viewport()->update();
    _currentFont=theFont;
}

void QHexMonitor::setLightStyle()
{
    setTextColor(Qt::black);
    setBgColor(QColor("#ffffff"));
    setHeaderColor(QColor("#d75a58"));
    setSelectColor(QColor("#d7621d"));
    setHeaderBgColor(QColor("#cecfc9"));
}

void QHexMonitor::setDarkStyle()
{
    setTextColor(QColor(85,255,85));
    setBgColor(QColor(33,33,33));
    setHeaderColor(Qt::yellow);
    setSelectColor(QColor("#a0a0a4"));
    setHeaderBgColor(QColor("#464847"));

}

void QHexMonitor::copyHex()
{
    QString str=getHexStr(getSelectDate());
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(str);
}

void QHexMonitor::copyStr()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(getAscllStr(getSelectDate()));
}

void QHexMonitor::setColumn()
{
    bool ok;
    int newCol=QInputDialog::getInt(this,"input with","with",_colCount,2,64,1,&ok);
    if(ok)setColumn(newCol);
}

void QHexMonitor::setFont()
{
   bool ok;
   QFont font= QFontDialog::getFont(&ok,_currentFont,this,"get word");
   if(ok)setFont(font);

}

void QHexMonitor::adjustColumn()
{
   int width=viewport()->width();
   int textWidth=_pxCharWidth*(4*(_colCount+1)+10);
   if(width>=textWidth)horizontalScrollBar()->setMaximum(0);
   else{
       int less=(textWidth-width)/_pxCharWidth;
       horizontalScrollBar()->setMaximum(less);
   }
}
QByteArray QHexMonitor::getSelectDate()
{
    int start=0;
    int size=0;
    if(_baseCursor>=0){
        if(_baseCursor!=_endCursor){
            int bia=_endCursor-_baseCursor;
            if(bia>0){
                start=_baseCursor;
                size=bia+1;
            }
            else{
                start=_endCursor;
                size=-bia+1;
            }
        }
        else{
            start=_baseCursor;
            size=1;
        }
        if(start>=_date.size()){
            start=0;
            size=0;
        }
    }
    return _date.mid(start,size);
}
QString QHexMonitor::getHexStr(QByteArray dat, char c)
{
    int size=dat.size();
    QByteArray hex(size * 3,c);
    char *hexData = hex.data();
    char *data = dat.data();
    for (int i = 0; i <size; ++i) {
        hexData[i*3] = "0123456789ABCDEF"[(data[i] >> 4) & 0xF];
        hexData[i*3+1] = "0123456789ABCDEF"[data[i]  & 0xF];
    }
    return hex;
}

QString QHexMonitor::getAscllStr(QByteArray dat, char c)
{
    int size=dat.size();
    QByteArray str(size,c);

    uchar ch;
    for (int i = 0; i <size; ++i){
        ch=(uchar)dat.at(i);
        if ( ch >= ' ' && ch <= '~' ){
            str[i]=ch;
        }
    }
    return str;
}

int QHexMonitor::getCursorIndex(QPointF pos)
{
    int res=-1;
    int base=verticalScrollBar()->value();
    int showBegin=base*_colCount;
    QPointF temp=pos-_topLeft;
    int width3=_pxCharWidth*3;

    int basey=horizontalScrollBar()->value();
    int leftSize=basey*_pxCharWidth;
    temp.rx()+=leftSize;


    int x=temp.x();
    int y=temp.y()/_pxCharHeight;
    int right=(_colCount*4+3)*_pxCharWidth;
    if(x<0 || x>right)x=0;//左侧或者右侧返回0
    else{
        if(x<_colCount*width3)x=x/width3;
        else x=(x-(_colCount+1)*width3)/_pxCharWidth;
        if(x<0)x=0;
    }
    res=showBegin + y*_colCount + x;
    if(res>_date.size())res=_date.size();
    return res;

}

void QHexMonitor::painSelectmark(QPainter *painter)
{
    if(_baseCursor>=0){
        int base=verticalScrollBar()->value();
        int showBegin=base*_colCount;
        int showSize=_date.size()-showBegin-1;
        int start,stop;
        if(_baseCursor<_endCursor){
            start=_baseCursor-showBegin;
            stop=_endCursor-showBegin;
        }
        else{
            start=_endCursor-showBegin;
            stop=_baseCursor-showBegin;
        }

        if(stop<0 || start>showSize)return;
        if(start<0)start=0;
        if(stop>showSize)stop=showSize;

       uint16_t startRow=start/_colCount;
       uint16_t stopRow=stop/_colCount;
       uint16_t rowCnt=stopRow-startRow;
       uint16_t strWidth3=3*_pxCharWidth;
       uint16_t hexStrWidth=strWidth3*(_colCount+1);//十六进制数据长度
        int basey=horizontalScrollBar()->value();
        int leftSize=basey*_pxCharWidth;
        QPoint realStartPoint=_markTopLeft-QPoint(leftSize,0);
       QPointF startPoint=realStartPoint+QPointF((start%_colCount)*strWidth3,startRow*_pxCharHeight);
       QPointF stoptPoint=realStartPoint+QPointF(0,stopRow*_pxCharHeight);
       QPointF strStartPoint=realStartPoint+QPointF(hexStrWidth+(start%_colCount)*_pxCharWidth,startRow*_pxCharHeight);
       QPointF strStopPoint=realStartPoint+QPointF(hexStrWidth,stopRow*_pxCharHeight);
       if(rowCnt==0){//同一行
           painter->fillRect(QRectF(startPoint,QSize((stop-start+1)*strWidth3,_pxCharHeight)),_selectColor);
           painter->fillRect(QRectF(strStartPoint,QSize((stop-start+1)*_pxCharWidth,_pxCharHeight)),_selectColor);

       }
       else if(rowCnt==1){//上下两行
            painter->fillRect(QRectF(startPoint,QSize((_colCount-(start%_colCount))*strWidth3,_pxCharHeight)),_selectColor);
            painter->fillRect(QRectF(stoptPoint,QSize((stop%_colCount+1)*strWidth3,_pxCharHeight)),_selectColor);

            painter->fillRect(QRectF(strStartPoint,QSize((_colCount-(start%_colCount))*_pxCharWidth,_pxCharHeight)),_selectColor);
            painter->fillRect(QRectF(strStopPoint,QSize((stop%_colCount+1)*_pxCharWidth,_pxCharHeight)),_selectColor);
       }
       else{
           QPointF firstPoint=realStartPoint+QPointF(0,(startRow+1)*_pxCharHeight);
           QPointF firstStrPoint=firstPoint+QPointF(hexStrWidth,0);
           painter->fillRect(QRectF(startPoint,QSize((_colCount-(start%_colCount))*strWidth3,_pxCharHeight)),_selectColor);
           painter->fillRect(QRectF(firstPoint,QSize(_colCount*strWidth3,_pxCharHeight*(rowCnt-1))),_selectColor);
           painter->fillRect(QRectF(stoptPoint,QSize((stop%_colCount+1)*strWidth3,_pxCharHeight)),_selectColor);

           painter->fillRect(QRectF(strStartPoint,QSize((_colCount-(start%_colCount))*_pxCharWidth,_pxCharHeight)),_selectColor);
           painter->fillRect(QRectF(firstStrPoint,QSize(_colCount*_pxCharWidth,_pxCharHeight*(rowCnt-1))),_selectColor);
           painter->fillRect(QRectF(strStopPoint,QSize((stop%_colCount+1)*_pxCharWidth,_pxCharHeight)),_selectColor);
       }
    }
}


void QHexMonitor::paintEvent(QPaintEvent *event)
{
     int basey=horizontalScrollBar()->value();
     int leftSize=basey*_pxCharWidth;
     int base=verticalScrollBar()->value();
     int showBegin=base*_colCount;
     int showSize=_showCount*_colCount;
     QByteArray showDate=_date.mid(showBegin,showSize);
     QPainter painter(viewport());
     int w=viewport()->width();
     int h=viewport()->height();

     int rowCount=_date.size()/_colCount;
     int maxSize=rowCount-_showCount+5;
     if(maxSize>0)verticalScrollBar()->setMaximum(maxSize);
     else verticalScrollBar()->setMaximum(0);

     if(_autoScroll){
         int maxRow=verticalScrollBar()->maximum();
         verticalScrollBar()->setValue(maxRow);
     }

     painter.fillRect(event->rect(),_bgColor);
     if(leftSize<_markTopLeft.x()){
         painter.fillRect(QRect(0,0,_markTopLeft.x()-leftSize,viewport()->height()),_headerBgColor);
     }
     painter.fillRect(QRect(0,0,viewport()->width(),_pxCharHeight),_headerBgColor);
     QString header,address;
     painter.setPen(QPen(_headerColor));
     painSelectmark(&painter);

     //绘制标记线
     QPoint leftConer=_markTopLeft;
     leftConer.rx()-=leftSize;
     painter.drawRect(QRect(leftConer,QSize(w+leftSize,h)));
     //绘制地址
     for(int i=0;i<_showCount;i++){
         address = QString("%1").arg((base+i)*_colCount, 8,16, QChar('0'));
         painter.drawText(QPoint(_pxCharWidth-leftSize,(i+2)*_pxCharHeight),address.toUpper());
     }
     QPoint startPoint=_topLeft-QPoint(leftSize,0);
     //绘制header
     for(int i=0;i<_colCount;i++){
         header.append(QString(" %1").arg(i, 2,16, QChar('0')));
     }
     painter.drawText(startPoint,header.toUpper());

     painter.setPen(QPen(_textColor));
     //绘制数据
     for(int i=0;i<_showCount;i++){
         painter.drawText(QPoint(startPoint.x()+_pxCharWidth,(i+2)*_pxCharHeight),getHexStr(showDate.mid(i*_colCount,_colCount)));
         painter.drawText(QPoint(startPoint.x()+_pxCharWidth*3*(_colCount+1),(i+2)*_pxCharHeight),getAscllStr(showDate.mid(i*_colCount,_colCount)));
     }
}

void QHexMonitor::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    _showCount=viewport()->height()/_pxCharHeight;
    adjustColumn();
    viewport()->update();

}

void QHexMonitor::mouseMoveEvent(QMouseEvent *e)
{
    _endCursor=getCursorIndex(e->localPos());
    if(e->localPos().y()>viewport()->height()){//down
        verticalScrollBar()->setValue(verticalScrollBar()->value()+1);
    }
    else if(e->localPos().y()<0){//up
        verticalScrollBar()->setValue(verticalScrollBar()->value()-1);
    }
    if(e->localPos().x()>viewport()->width()){//right
        horizontalScrollBar()->setValue(horizontalScrollBar()->value()+1);
    }
    else if(e->localPos().x()<0){
        horizontalScrollBar()->setValue(horizontalScrollBar()->value()-1);
    }
    viewport()->update();
}

void QHexMonitor::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton){
        _baseCursor=getCursorIndex(e->localPos());
        _endCursor=_baseCursor;
        viewport()->update();
    }
}

void QHexMonitor::contextMenuEvent(QContextMenuEvent *event)
{
    //菜单出现的位置为当前鼠标的位置
    pop_menu->exec(QCursor::pos());
    event->accept();
}
