#include "screen.h"
#include "ui_screen.h"
#include <QGuiApplication>
#include <QDir>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QApplication>
#include <QTimer>

Screen::Screen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Screen)
{
    ui->setupUi(this);
    showFullScreen(); //将窗口部件全屏显示

    // 处理多屏幕问题，在鼠标所在的屏幕上截图
//    QScreen *scrPix = QGuiApplication::primaryScreen();
    QScreen *scrPix = QGuiApplication::screenAt(QCursor::pos());
    //截取全屏
    pixmap = scrPix->grabWindow(0);

    resolution = 1.0 * pixmap.width() / this->width();

    //全局路径的初始化，一个全屏闭合回路
    globalPath.lineTo(pixmap.width() / resolution, 0);
    globalPath.lineTo(pixmap.width() / resolution, pixmap.height() / resolution);
    globalPath.lineTo(0, pixmap.height() / resolution);
    globalPath.lineTo(0, 0);

    setMouseTracking(true);

    labelimage=new QSLabel(this);
    Qt::WindowFlags nType = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;
    labelimage->setWindowFlags(nType);
    labelimage->setParent(this);
}

Screen::~Screen()
{
    delete ui;
}

void Screen::paintEvent(QPaintEvent *e)
{
    QPainter paint(this);
    paint.drawPixmap(0, 0, pixmap);
    //初始化画笔操作
    paint.setPen(Qt::blue);
    paint.setBrush(QColor(0, 0, 0, 100));
    //设置路径
    QPainterPath path = getPath();

    paint.drawPath(path);

    //绘制选取左上角显示宽高的区域
    widthInfoRect.drawMe(paint);

    //绘制四个小正方形
    drawControlArea(paint);

//    labelimage->drawMe(paint);
}

//绘制正方形
void Screen::drawControlArea(QPainter &painter)//已看懂
{
    //计算四个小正方形
    rect1.setX(movePoint.x() - 3);
    rect1.setY(pressedPoint.y() - 3);
    rect1.setWidth(6);
    rect1.setHeight(6);
    rect2.setX(pressedPoint.x() - 3);
    rect2.setY(pressedPoint.y() - 3);
    rect2.setWidth(6);
    rect2.setHeight(6);
    rect3.setX(pressedPoint.x() - 3);
    rect3.setY(movePoint.y() - 3);
    rect3.setWidth(6);
    rect3.setHeight(6);
    rect4.setX(movePoint.x() - 3);
    rect4.setY(movePoint.y() - 3);
    rect4.setWidth(6);
    rect4.setHeight(6);
    painter.save();
    painter.setBrush(Qt::blue);
    painter.drawRect(rect1);
    painter.drawRect(rect2);
    painter.drawRect(rect3);
    painter.drawRect(rect4);
    painter.restore();
}

void Screen::mousePressEvent(QMouseEvent *e)
{
    if( e->button() & Qt::LeftButton && oncePress) //如果鼠标左键第一次按下
    {
       pressedPoint = e->pos();
    }
    else
    {
        if( control )  //如果控制面板已经出现，则第二次以后的每一次鼠标按键都会使其暂时隐藏
            control->hide();
        oldPoint = e->pos();
    }
}

void Screen::mouseMoveEvent(QMouseEvent *e)
{
    setselectimagelabel(e);
    if(e->buttons() & Qt::LeftButton)
    {
        //选取左上角的信息更改
        minX = pressedPoint.x() <= movePoint.x() ? pressedPoint.x() : movePoint.x();
        maxX = pressedPoint.x() <= movePoint.x() ? movePoint.x() : pressedPoint.x();
        minY = pressedPoint.y() <= movePoint.y() ? pressedPoint.y() : movePoint.y();
        maxY = pressedPoint.y() <= movePoint.y() ? movePoint.y() : pressedPoint.y();
        widthInfoRect.setText(tr("%1 * %2").arg((maxX - minX) * resolution).arg((maxY - minY) * resolution));   //将截图选区的长宽信息显示在widthinforect中
        widthInfoRect.setLocation(minX, minY);
        rect.setX(minX);
        rect.setY(minY);
        rect.setWidth(maxX - minX);
        rect.setHeight(maxY - minY);
        if(oncePress)
        {
            movePoint = e->pos();
        }
        else
        {
            QPoint tempPoint = e->pos();     //当前鼠标位置
            int moveX = tempPoint.x() - oldPoint.x();  //鼠标移动的x距离
            int moveY = tempPoint.y() - oldPoint.y();   //鼠标移动的y距离
            if(isDragRect)
            {
                isDragRect = true;
                int tempPressX = pressedPoint.x() + moveX;
                int tempPressY = pressedPoint.y() + moveY;
                int tempMoveX = movePoint.x() + moveX;
                int tempMoveY = movePoint.y() + moveY;
                int deskWidth = pixmap.width();
                int deskHeight = pixmap.height();
                if( tempPressX < 0 )
                {
                    tempPressX = 0;
                    tempMoveX = movePoint.x();
                }
                if( tempPressX > deskWidth)
                {
                    tempPressX = deskHeight;
                }
                if(tempPressY < 0)
                {
                    tempPressY = 0;
                    tempMoveY = movePoint.y();
                }
                if( tempPressY > deskHeight)
                {

                    tempPressY = deskHeight;
                }
                if( tempMoveX < 0)
                {
                    tempMoveX = 0;
                }
                if( tempMoveX > deskWidth)
                {
                    tempMoveX = deskWidth;
                    tempPressX = pressedPoint.x();
                }
                if( tempMoveY < 0)
                {
                    tempMoveY = 0;
                }
                if( tempMoveY > deskHeight)
                {
                    tempMoveY = deskHeight;
                    tempPressY = pressedPoint.y();

                }
                pressedPoint.setX(tempPressX);    //鼠标在截图区域中拖动结束后，选区的位置
                pressedPoint.setY(tempPressY);
                movePoint.setX(tempMoveX);
                movePoint.setY(tempMoveY);
            }
            else {
                switch( type )         //选区角落四个小矩形的位置
                {
                    case RECT1:               //意思是第一次选区之后，分别拖动四个角落的小矩形时候，截图选区的变化
                        pressedPoint.setY(pressedPoint.y() + moveY);  //右上角的矩形
                        movePoint.setX(movePoint.x() + moveX);
                        break;
                    case RECT2:
                        pressedPoint.setX(pressedPoint.x() + moveX);    //左上角的矩形
                        pressedPoint.setY(pressedPoint.y() + moveY);
                        break;
                    case RECT3:
                        pressedPoint.setX(pressedPoint.x() + moveX);  //左下角的矩形
                        movePoint.setY(movePoint.y() + moveY);
                        break;
                    case RECT4:
                        movePoint.setX(movePoint.x() + moveX);     //右下角的矩形
                        movePoint.setY(movePoint.y() + moveY);
                        break;
                    case RECT:                  //指的是当鼠标在截图选区中按下左键然后拖动时候，截图选区的整体位置变化
                    {
                        isDragRect = true;
                        int tempPressX = pressedPoint.x() + moveX;
                        int tempPressY = pressedPoint.y() + moveY;
                        int tempMoveX = movePoint.x() + moveX;
                        int tempMoveY = movePoint.y() + moveY;
                        int deskWidth = pixmap.width();
                        int deskHeight = pixmap.height();
                        if( tempPressX < 0 )
                        {
                            tempPressX = 0;
                            tempMoveX = movePoint.x();
                        }
                        if( tempPressX > deskWidth)
                        {
                            tempPressX = deskHeight;
                        }
                        if(tempPressY < 0)
                        {
                            tempPressY = 0;
                            tempMoveY = movePoint.y();
                        }
                        if( tempPressY > deskHeight)
                        {

                            tempPressY = deskHeight;
                        }
                        if( tempMoveX < 0)
                        {
                            tempMoveX = 0;
                        }
                        if( tempMoveX > deskWidth)
                        {
                            tempMoveX = deskWidth;
                            tempPressX = pressedPoint.x();
                        }
                        if( tempMoveY < 0)
                        {
                            tempMoveY = 0;
                        }
                        if( tempMoveY > deskHeight)
                        {
                            tempMoveY = deskHeight;
                            tempPressY = pressedPoint.y();

                        }
                        pressedPoint.setX(tempPressX);    //鼠标在截图区域中拖动结束后，选区的位置
                        pressedPoint.setY(tempPressY);
                        movePoint.setX(tempMoveX);
                        movePoint.setY(tempMoveY);
                        break;
                    }
                    case NO:
                        break;
                    default:
                        break;
                }
            }
            oldPoint = tempPoint;    //更新位置坐标信息
        }
        repaint();
    }
    if(isDragRect)
    {
        setCursor(Qt::SizeAllCursor);
    }
    else{
        Type r = pointInWhere(e->pos());
        if( r == RECT1)
        {
            type = RECT1;
            comparePoint(pressedPoint, movePoint) ? setCursor(Qt::SizeBDiagCursor) : setCursor(Qt::SizeFDiagCursor);
        } else if( r == RECT2)
        {
            type = RECT2;
            comparePoint(pressedPoint, movePoint) ? setCursor(Qt::SizeFDiagCursor) : setCursor(Qt::SizeBDiagCursor);
        } else if( r == RECT3)
        {
            type = RECT3;
            comparePoint(pressedPoint, movePoint) ? setCursor(Qt::SizeBDiagCursor) : setCursor(Qt::SizeFDiagCursor);
        } else if( r == RECT4)
        {
            type = RECT4;
            comparePoint(pressedPoint, movePoint) ? setCursor(Qt::SizeFDiagCursor) : setCursor(Qt::SizeBDiagCursor);
        } else if( r == RECT)
        {
            type = RECT;
            setCursor(Qt::SizeAllCursor);
        } else
        {
            type = NO;
            setCursor(Qt::ArrowCursor);
        }
    }
}

void Screen::mouseReleaseEvent(QMouseEvent *e)
{
    isDragRect = false;
    oncePress = false;
    if( e->button() & Qt::LeftButton)
    {
        control = new QWidget(this);         //新建一个窗口控件
        controlUi = new ControlWidget(control);  //新建控制窗口
        QHBoxLayout *layout = new QHBoxLayout(control);    //在control上建立水平布局
        layout->addWidget(controlUi);         //将控制窗口应用于水平布局
        layout->setContentsMargins(0, 0, 0, 0);
        control->setObjectName("control");
        control->setStyleSheet("QWidget#control{\
                               background-color: #eaecf0;}");
        controlUi->setScreenQuote(this);
        // 当控制条出屏幕底部时，调整显示
        if(maxY + 31 > this->height())
        {
            control->setGeometry(maxX - 400, maxY - 31, 400, 25);
        }
        else
        {
            control->setGeometry(maxX - 400, maxY + 6, 400, 25);
        }
        control->show();
        control->setCursor(Qt::PointingHandCursor);
    }
}

//得到选区之外的区域作为相应蒙层
QPainterPath Screen::getPath()
{
    //选取路径
    QPainterPath path;
    path.moveTo(pressedPoint.x(), pressedPoint.y());
    path.lineTo(movePoint.x(), pressedPoint.y());
    path.lineTo(movePoint.x(), movePoint.y());
    path.lineTo(pressedPoint.x(), movePoint.y());
    path.lineTo(pressedPoint.x(), pressedPoint.y());
    return globalPath.subtracted(path);
}

QImage Screen::getGrabPixmap()   //返回截到的图片
{
    return labelimage->resultimage();
}

//保存截取出来的图片
void Screen::savePixmap()
{
    //生成图片名称
    QString picName;
    QTime time;
    //获取当前系统时间，用做伪随机数的种子
    time = QTime::currentTime();
    srand(time.msec() + time.second() * 1000);
    //随机字符串
    QString randStr;
    randStr.setNum(rand());
    picName.append(randStr);

    QString filename=QFileDialog::getSaveFileName(this,QStringLiteral("保存截图"),picName,"JPEG Files(*.jpg)");

    if(filename.length()>0){
        QImage pimage=labelimage->resultimage();
        pimage.save(filename, "jpg");
    }
}

//计算此时鼠标移动在哪个选区上面
Type Screen::pointInWhere(QPoint p)
{
    if( pointInRect(p, rect1))
        return RECT1;
    else if( pointInRect(p, rect2))
        return RECT2;
    else if( pointInRect(p, rect3))
        return RECT3;
    else if( pointInRect(p, rect4))
        return RECT4;
    else if( pointInRect(p, rect))
        return RECT;
    else
        return NO;

}

//判断点是否在矩形之内
bool Screen::pointInRect(QPoint &p, QRectF &r)
{
    if( p.x() > r.x() && p.x() < r.x() + r.width() &&
            p.y() > r.y() && p.y() < r.y() + r.height())
        return true;
    return false;
}

bool Screen::comparePoint(QPoint &p1, QPoint &p2)
{
    if((p1.x() < p2.x() && p1.y() < p2.y()) || (p1.x() > p2.x() && p1.y() > p2.y()))
        return true;
    return false;
}

void Screen::keyPressEvent(QKeyEvent *e)    //键盘事件，包括esc退出截图，回车键完成截图，返回键完成截图
{
    int key = e->key();
    switch( key )
    {
    case Qt::Key_Escape :
        close();
        QTimer::singleShot(2000, this, [=](){
            this->deleteLater();
        });
        break;
    case Qt::Key_Enter:
        if( controlUi )
        {
            controlUi->finishBtn_slot();
        }
        break;
    case Qt::Key_Return :
        if( controlUi )
        {
            controlUi->finishBtn_slot();
        }
        break;
    default:
        break;
    }
}

void Screen::setselectimagelabel(QMouseEvent* event)
{
        int wid = abs(movePoint.x() - pressedPoint.x());
        int hei = abs(movePoint.y() - pressedPoint.y());
        int x =pressedPoint.x() < movePoint.x() ? pressedPoint.x() : movePoint.x();
        int y =pressedPoint.y() < movePoint.y() ? pressedPoint.y() : movePoint.y();
        QImage selectimage=pixmap.copy(x * resolution,y * resolution,wid * resolution,hei * resolution).toImage();

        labelimage->setimagetolabel(selectimage);
        labelimage->setFixedSize(wid,hei);
        labelimage->move(QPoint(x,y));
        labelimage->show();
}

void Screen::drawrectang()
{
    labelimage->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    labelimage->setrectangleenable();
}

void Screen::drawround()
{
    labelimage->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    labelimage->setroundenable();
}

void Screen::drawarrow()
{
    labelimage->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    labelimage->setdrawarrowenable();
}

void Screen::drawline()
{
    labelimage->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    labelimage->setdrawlineenable();
}

void Screen::textedit()
{
    labelimage->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    labelimage->settexteditenable();
}

void Screen::Exit()
{
    if(labelimage)
    {
        labelimage->close();
    }
}
