#ifndef PID_WIDGET_H
#define PID_WIDGET_H

#include <QWidget>
#include <Qtimer.h>

#include <QPainter>     //画图笔
#include <QDebug>
#include <QVector>      //数据顺序容器

class pid_widget : public QWidget
{
    Q_OBJECT
public:
    explicit pid_widget(QWidget *parent = nullptr);
    //~pid_widget();

    /*-----------------------varirable------------------------*/
    QVector<QPoint> yaw_Point_array;        //保存所有坐标点的顺序容器
    QVector<QPoint> distance_Point_array;
private:
    /*-----------------------function------------------------*/
    void paintEvent(QPaintEvent *event);        //绘图函数，在界面刷新（移动，缩小，放大）的时候自动调用


    /*-----------------------varirable------------------------*/
//    QTimer *timer;      //定时器


signals:



//public slots:
//    void timer_timerout();                  //定时器处理函数

};

#endif // PID_WIDGET_H
