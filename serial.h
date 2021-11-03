#ifndef SERIAL_H
#define SERIAL_H

#include <QMainWindow>

/*-------------------User----------------------*/
//Port
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <qstring.h>
#include <Qtime>
#include "pid_widget.h"
#include <Qtimer>
#include <QVector>      //数据顺序容器

//Debug
#include <QDebug>
/*---------------------------------------------*/

namespace Ui {
class serial;
}

class serial : public QMainWindow
{
    Q_OBJECT

public:
    explicit serial(QWidget *parent = nullptr);     //显式构造函数
    ~serial();                                      //析构函数

private slots:
    void on_openButton_clicked();   //打开串口槽函数

    void on_closeButton_clicked();  //关闭串口槽函数

    void on_sendButton_clicked();   //发送数据槽函数

    int receive_data();

    void on_clearButton_clicked();

    void on_stopButton_clicked();

    void speedDialControl();

    void angularSpeedDialControl();

    void modeButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::serial *ui;
    /*-----------------------function------------------------*/
    void system_init();
    char ConvertHexChar(char ch);
    QByteArray QString2Hex(QString str);
//    void paintEvent(QPaintEvent *event);        //绘图函数，在界面刷新（移动，缩小，放大）的时候自动调用


    /*-----------------------varirable------------------------*/
    QSerialPort *serialPort;
    pid_widget *widget;
    QTime m_time;
    QByteArray nrfArray;// = {"\x07\x06\x01\x64\x64\xA0\x01\x64\x64\xA0",10};

};

#endif // SERIAL_H
