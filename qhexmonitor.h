#ifndef QHEXMONITOR_H
#define QHEXMONITOR_H

#include <QAbstractScrollArea>
#include <QPen>
#include <QBrush>
#include <qpainter.h>
#include <QPaintEvent>
#include <QStringList>
#include <QMenu>
#include <QAction>
#define MIN_COL_COUNT 2
#define MAX_COL_COUNT 64
class QHexMonitor : public QAbstractScrollArea
{
    Q_OBJECT
public:
    explicit QHexMonitor(QWidget *parent = 0);
    void setColumn(int count);
    QByteArray getDate(){return _date;}
    int getDateSize(){return _date.size();}

 public slots:
    void append(QByteArray ba);
    void setFont(const QFont &font);
    void setLightStyle();
    void setDarkStyle();

    //设置颜色
    void setTextColor(QColor color){_textColor=color;}
    void setBgColor(QColor color){_bgColor=color;}
    void setHeaderColor(QColor color){_headerColor=color;}
    void setHeaderBgColor(QColor color){_headerBgColor=color;}
    void setSelectColor(QColor color){_selectColor=color;}

    //菜单
    void copyHex();
    void copyStr();
    void selectAll(){_baseCursor=0;_endCursor=_date.size();viewport()->update();}
    void clearAll(){_date.clear();_baseCursor=-1;viewport()->update();}
    void setAutoScroll(bool ok){_autoScroll=ok;}
    void setColumn();
    void setFont();


private:
    QByteArray _date;
    uint16_t _pxCharWidth,_pxCharHeight;
    uint16_t _colCount=16;
    uint16_t _showCount;
    QPoint _topLeft;
    QPoint _markTopLeft;
    bool _autoScroll=false;
    int _baseCursor=-1;//光标选中位置
    int _endCursor;
    QFont _currentFont;

    QColor _textColor;
    QColor _bgColor;
    QColor _headerColor;
    QColor _headerBgColor;
    QColor _selectColor;

    QMenu   *pop_menu;
    QAction *actionCopyHex;
    QAction *actionCopyStr;
    QAction *actionSelectAll;
    QAction *actionClear;
    QAction *actionAutoScroll;
    QAction *actionSetColumn;

    void adjustColumn();

private:

    QString getHexStr(QByteArray dat,char c=' ');
    QString getAscllStr(QByteArray dat,char c='.');
    int getCursorIndex(QPointF pos);
    void painSelectmark(QPainter *painter);
    QByteArray getSelectDate();

    void createActions();
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void contextMenuEvent(QContextMenuEvent *event);

};

#endif // QHEXMONITOR_H
