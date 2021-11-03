#include "pid_widget.h"

pid_widget::pid_widget(QWidget *parent) : QWidget(parent)
{
//    qsrand(100);
//    /*创建定时器*/
//    timer = new QTimer(this);


//    /*---------------------------------------槽函数连接----------------------------------------*/
//    //connect(信号发出者的地址，发出的信号，接收者的地址（在哪个类触发），触发方法即槽函数)
//    connect(timer,SIGNAL(timeout()),this,SLOT(timer_timerout()));//&serial::timer_timerout


//    /*启动定时器*/
//    timer->start(100);
}


/*--------------------------------------------------------------
 *          functions
 *--------------------------------------------------------------*/
void pid_widget::paintEvent(QPaintEvent *event)
{
    /*画笔初始化*/
    QPainter painter(this);

    QPen yaw_pen;
    yaw_pen.setWidth(1);
    yaw_pen.setBrush(QBrush(Qt::red));

    QPen distance_pen;
    distance_pen.setWidth(1);
    distance_pen.setBrush(QBrush(Qt::green));


    painter.translate(0,this->height());    //平移原点
    painter.scale(1,2);                       //缩放

    int i;
    for(i = 1; i < yaw_Point_array.size(); i++)     //两个容器大小一样
    {
        QPoint p1 = yaw_Point_array[i-1];
        QPoint p2 = yaw_Point_array[i];
        painter.setPen(yaw_pen);
        painter.drawLine(p1 , p2);
        QPoint p3 = distance_Point_array[i-1];
        QPoint p4 = distance_Point_array[i];
        painter.setPen(distance_pen);
        painter.drawLine(p3 , p4);
    }

}



/*--------------------------------------------------------------
 *          slots
 *--------------------------------------------------------------*/

/*定时器产生实时数据*/
//void pid_widget::timer_timerout()
//{
//    static int x = 0;
//    int y;
//    if(x < this->width())
//    {
//        y = rand()%100;
//        Point_array.push_back(QPoint(x,y));
//        x += 5;
//    }
//    else    //超出界面范围的x
//    {
//        int i;
//        for(i = 1; i < Point_array.size(); i++)
//        {
//            Point_array[i-1].setY(Point_array[i].y());
//        }
//        Point_array[Point_array.size()-1].setY(rand()%100);

//    }

//    this->update();
//}

