#ifndef SCREEN_H
#define SCREEN_H

#include <QWidget>
#include <QScreen>
#include <QPainterPath>
#include "controlwidget.h"
#include "myrect.h"
#include "qslabel.h"

enum Type {
    RECT1, RECT2, RECT3, RECT4, RECT, NO
};

QT_BEGIN_NAMESPACE
namespace Ui { class Screen; }
QT_END_NAMESPACE

class Screen : public QWidget
{
    Q_OBJECT

public:
    Screen(QWidget *parent = nullptr);
    ~Screen();

    void paintEvent(QPaintEvent *);

    void mousePressEvent(QMouseEvent *);

    void mouseReleaseEvent(QMouseEvent *);

    void mouseMoveEvent(QMouseEvent *);

    QPainterPath getPath();

    //返回截取之后的图片
    QImage getGrabPixmap();

    //保存截图
    void savePixmap();

    //绘制控制选区，也就是四个小三角
    void drawControlArea(QPainter &);

    Type pointInWhere(QPoint p);

    bool comparePoint(QPoint &p1, QPoint &p2);

    void keyPressEvent(QKeyEvent *e);

    void setselectimagelabel(QMouseEvent* event);

    void drawrectang();
    void drawround();
    void drawarrow();
    void drawline();
    void textedit();

    void Exit();

private:
    Ui::Screen *ui;
    QPixmap pixmap;               //全屏
    QPoint pressedPoint;          //鼠标左键按下后的坐标
    QPoint movePoint;             //终点坐标
    QPoint oldPoint;        //原先的坐标
    double resolution;    // 屏幕分辨率（图片像素与屏幕坐标之间的比值）
    QPainterPath globalPath;      //保存全局路径
    QWidget *control;
    ControlWidget *controlUi;
    MyRect widthInfoRect;//截取区域的长宽信息矩形
    int minX;
    int minY;
    int maxX;
    int maxY;
    //控制选区变化的正方形
    QRectF rect1;           //第一象限的正方形
    QRectF rect2;           //第二象限的正方形
    QRectF rect3;           //第三象限的正方形
    QRectF rect4;           //第四象限的正方形
    QRectF rect;            //代表选区的正方形
    Type type;
    bool oncePress = true;       //是不是第一次左键按下

    bool   pointInRect(QPoint &p, QRectF &r);

    QSLabel *labelimage;
};
#endif // SCREEN_H
