#include "serial.h"
#include "ui_serial.h"
#include "pid_widget.h"

serial::serial(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::serial)
{
    ui->setupUi(this);
    /*User*/
    //QByteArray nrfArray;
    system_init();
    this->setWindowTitle("serial");
    widget = new pid_widget(this);
    ui->tabWidget->addTab(widget,"pid_curve");
}

serial::~serial()
{
    delete ui;
}

/*--------------------------------------------------------------
 *          functions
 *--------------------------------------------------------------*/
void serial::system_init()
{
    /*创建一个串口对象*/
    serialPort = new QSerialPort(this);

    /*串口名列表*/
    QStringList serialNamePort;

    /*初始化QByteArray*/
    nrfArray.resize(10);
    nrfArray[0] = 0x07;
    nrfArray[1] = 0x06;
    nrfArray[2] = 0x01;     //Mode
    nrfArray[3] = 0x64;     //AngularSpeed
    nrfArray[4] = 0x64;     //Speed
    nrfArray[5] = 0xA0;
    nrfArray[6] = 0x01;
    nrfArray[7] = 0x64;     //AS
    nrfArray[8] = 0x64;     //S
    nrfArray[9] = 0xA0;

    /*---------------搜索所有可用串口----------------*/
    foreach (const QSerialPortInfo &inf0, QSerialPortInfo::availablePorts()){
        serialNamePort<<inf0.portName();
    };
    /*----------将可用串口名添加到列表---------*/
    ui->serialBox->addItems(serialNamePort);

    /*-----------------port config-------------------*/
    /*------写死数据位、停止位、校验位------*/
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);

    /*--------------------仪表盘初始化---------------------*/
    ui->speedDial->setMinimum(1);
    ui->speedDial->setMaximum(200);
    ui->speedDial->setSliderPosition(100);


    ui->angularSpeedDial->setMinimum(1);
    ui->angularSpeedDial->setMaximum(200);
    ui->angularSpeedDial->setSliderPosition(100);



    /*---------------------------------------槽函数连接----------------------------------------*/
    //connect(信号发出者的地址，发出的信号，接收者的地址（在哪个类触发），触发方法即槽函数)
    connect(serialPort,&QSerialPort::readyRead,this,&serial::receive_data);
    connect(ui->speedDial,&QDial::valueChanged,this,&serial::speedDialControl);
    connect(ui->angularSpeedDial,&QDial::valueChanged,this,&serial::angularSpeedDialControl);
    //connect(ui->angularSpeedDial,SIGNAL(sliderMoved(int)),this,&serial::angularSpeedDialControl);
    connect(ui->modeButton,&QPushButton::clicked,this,&serial::modeButton_clicked);


}
/*-----------------------字符串转十六进制----------------------------*/


char serial::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

QByteArray serial::QString2Hex(QString str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}




/*--------------------------------------------------------------
 *          slots
 *--------------------------------------------------------------*/

/*----------------------打开串口按钮---------------------*/

void serial::on_openButton_clicked()
{
//    qDebug()<<"ok";               //类似C++ cout写法
//    qDebug("debug: %d\r\n456",123);      //类似C printf写法
//    /*选项卡使用*/
//    qDebug("baudrate_Index:%d",ui->baudrateBox->currentIndex());    //测试选项框不同条目的编号
//    switch (ui->baudrateBox->currentIndex()) {
//    case 0:

//        break;
//    case 1:

//        break;
//    default:
//        break;
//    }

    /* 串口设置 */
    serialPort->setPortName(ui->serialBox->currentText());          //端口号读取
    serialPort->setBaudRate(ui->baudrateBox->currentText().toInt());    //波特率读取
    serialPort->open(QIODevice::ReadWrite);     //打开串口
    ui->statusLab->setText("Connected");

//    /* 打开串口提示框 */
//    if (true == serialPort->open(QIODevice::ReadWrite))
//    {
//        QMessageBox::information(this, "提示", "串口打开成功");
//    }
//    else
//    {
//        QMessageBox::critical(this, "提示", "串口打开失败");
//    }

    //serialPort->write("123");   //写串口
}
/*----------------------关闭串口按钮---------------------*/
void serial::on_closeButton_clicked()
{
    serialPort->close();
    ui->statusLab->setText("Disconnected");
}
/*-----------------------发送按钮----------------------*/
void serial::on_sendButton_clicked()
{
    QString sendData = ui->sendData->text();
    QByteArray array = serial::QString2Hex(sendData);
    //QByteArray array = sendData.toLatin1();
    //serialPort->write(array);             //以‘\0’结尾的字符串中的数据写入设备（‘\0’以后的数据都丢掉了）。返回实际写入的字节数；如果发生错误，则返回-1。
    serialPort->write(array,qstrlen(array));        //将最多maxSize字节数据写入设备。返回实际写入的字节数；如果发生错误，则返回-1。
}
/*--------------------接收数据读取及显示-----------------*/
int serial::receive_data()
{
    QByteArray array = serialPort->readAll();

    QByteArray Yaw_byte;
    QByteArray distance_byte;

    float yaw_actual;
    float distance_actual;
    /*数据校验解析*/
    int i = 0;
    if((char)array[0]=='A'&&(char)array[1]=='B')
    {
        for(i =2; i < 10; i++)
        {
            if(array[i] != array[i+8])return 0;
            else
            {
                if(i < 6)Yaw_byte[i-2] = array[i];
                else distance_byte[i-6] = array[i];
            }
        }
//        for(i =2; i < 6; i++)
//        {
//            if(array[i] != array[i+4])return 0;
//            else Yaw_byte[i-2] = array[i];
//        }
        memcpy(&yaw_actual,Yaw_byte,sizeof(yaw_actual));                    //byte转float
        memcpy(&distance_actual,distance_byte,sizeof(distance_actual));     //byte转float
        qDebug()<<Yaw_byte.toHex();
        qDebug()<<"航向角："<<yaw_actual<<"    距离："<<distance_actual<<endl;
        //qDebug("%f\r\n",yaw_actual);
    }
    /*曲线点生成*/
    static int x = 0;
    int yaw_y;
    int distance_y;
    if(x < this->width())
    {
        yaw_y = -(int)yaw_actual;
        distance_y = -(int)distance_actual*10;
        widget->distance_Point_array.push_back(QPoint(x,distance_y));
        widget->yaw_Point_array.push_back(QPoint(x,yaw_y));
        x += 5;
    }
    else    //超出界面范围的x
    {
        int i;
        for(i = 1; i < widget->yaw_Point_array.size(); i++)
        {
            widget->yaw_Point_array[i-1].setY(widget->yaw_Point_array[i].y());
            widget->distance_Point_array[i-1].setY(widget->distance_Point_array[i].y());
        }
        widget->yaw_Point_array[widget->yaw_Point_array.size()-1].setY(-(int)yaw_actual);
        widget->distance_Point_array[widget->distance_Point_array.size()-1].setY(-(int)distance_actual*10);

    }
    this->update();


    QString framedata = QString(array);
    m_time = QTime::currentTime();
    QString time_str = m_time.toString("HH:mm:ss:zzz");
    framedata = QString("[%1]:RX-> %2").arg(time_str).arg(framedata);
    qDebug()<<framedata;
//    framedata = QString("[%1]:RX-> %2").arg(Qtime::currentTime().toString("HH:mm:ss:zzz")).arg(framedata);
//    qDebug(framedata);
    if(ui->hexDisplayBox->checkState() == Qt::Checked){
        ui->receiveData->insertPlainText(QString(array.toHex(' ')).append(' '));
    }
    else{
        ui->receiveData->insertPlainText(QString(array));
    }
    return 0;
}
/*---------------------清空接收区按钮--------------------*/
void serial::on_clearButton_clicked()
{
    ui->receiveData->clear();
}
/*--------------------制动按钮-------------------------*/
void serial::on_stopButton_clicked()
{
    nrfArray[2] = 0x02;
    nrfArray[6] = 0x02;
    serialPort->write(nrfArray,qstrlen(nrfArray));
    ui->modeLab->setText("制动");
}
/*-----------------------调速旋钮----------------------*/
void serial::speedDialControl()
{
    if("手动" != ui->modeLab->text())
    {
        QString speedValue(QString::number(ui->speedDial->value()));
        ui->speedLab->setText(QString::number(speedValue.toInt()-100));
        char value = (char)speedValue.toInt();
        nrfArray[4] = value;
        nrfArray[8] = value;
        //qDebug()<<nrfArray[4];
        QMessageBox::critical(this, "警告", "请先调整旋钮值，并转换为手动模式");
    }
    else
    {
        QString speedValue(QString::number(ui->speedDial->value()));
        ui->speedLab->setText(QString::number(speedValue.toInt()-100));
    //    qDebug()<<speedValue;
    //    QByteArray array = speedValue.toLatin1(); //每次读取得到一个十进制数，十进制数以字符串格式存储，如"123"，
    //    转为QByteArray时，array[0]="1",array[1]="2",array[2]="3"
    //    serialPort->write(array,qstrlen(array));
        char value = (char)speedValue.toInt();
        nrfArray[4] = value;
        nrfArray[8] = value;
        serialPort->write(nrfArray,qstrlen(nrfArray));
//        for(int i=0;i<10;i++)
//        {
//            qDebug("%d ",(char)nrfArray[i]);
//        }
//        qDebug()<<"------------------";
    }

}
/*-----------------------转向旋钮----------------------*/
void serial::angularSpeedDialControl()
{
    if("手动" != ui->modeLab->text())
    {
        QString angularSpeedValue(QString::number(ui->angularSpeedDial->value()));
        ui->angularSpeedLab->setText(QString::number(angularSpeedValue.toInt()-100));
        char value = (char)angularSpeedValue.toInt();
        nrfArray[3] = value;
        nrfArray[7] = value;
        QMessageBox::critical(this, "警告", "请先调整旋钮值，并转换为手动模式");
    }
    else
    {
        QString angularSpeedValue(QString::number(ui->angularSpeedDial->value()));
        ui->angularSpeedLab->setText(QString::number(angularSpeedValue.toInt()-100));
        //qDebug()<<angularSpeedValue;
        char value = (char)angularSpeedValue.toInt();
        nrfArray[3] = value;
        nrfArray[7] = value;
        serialPort->write(nrfArray,qstrlen(nrfArray));
    }

}
/*----------------------------------------
 *模式选择按钮   01：手动  02：制动  03：自动
 *----------------------------------------*/
void serial::modeButton_clicked()
{
    QString lastMode = ui->modeLab->text();
    if(lastMode == "手动")        //手动转自动
    {
        nrfArray[2] = 0x03;
        nrfArray[6] = 0x03;
        serialPort->write(nrfArray,qstrlen(nrfArray));
        ui->modeLab->setText("自动");
    }
    else if(lastMode == "自动")   //自动转手动
    {
        nrfArray[2] = 0x01;
        nrfArray[6] = 0x01;
        serialPort->write(nrfArray,qstrlen(nrfArray));
        ui->modeLab->setText("手动");
    }
    else                     //制动转手动
    {
        nrfArray[2] = 0x01;
        nrfArray[6] = 0x01;
        serialPort->write(nrfArray,qstrlen(nrfArray));
        ui->modeLab->setText("手动");
    }
    QString mode = ui->modeLab->text();
    //qDebug()<<"lastMode:"<<lastMode<<"currentMode:"<<mode;
}

void serial::on_pushButton_clicked()
{
    nrfArray[5] = 0xB0;
    nrfArray[9] = 0xB0;
    serialPort->write(nrfArray,qstrlen(nrfArray));
    nrfArray[5] = 0xA0;
    nrfArray[9] = 0xA0;
}

