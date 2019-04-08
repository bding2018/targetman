#include "qtargetserial.h"
#include "power_modbus.h"
QTargetSerial::QTargetSerial(QObject *parent, TTarget* pTarget) : QSerialPort((QObject*)parent)
{
	m_mainWindow = NULL;
	m_target = pTarget;


}
QTargetSerial::~QTargetSerial()
{

}

void QTargetSerial::slotDisconnected()
{

}

QStringList QTargetSerial::getSerialPort()
{
	//QList<QSerialPortInfo> QSerialPortInfo::availablePorts()

	QStringList lstSerialPortName;
	for each(const QSerialPortInfo &info in QSerialPortInfo::availablePorts())
	{
		lstSerialPortName << info.portName();
		qDebug() << "serialPortName:" << info.portName();
	}
	return lstSerialPortName;
}

bool QTargetSerial::openSerialPort(const QString& portName,
							BaudRate baudRate ,
							Directions direction ,
							DataBits datBits ,
							FlowControl flowControl,
							Parity parity ,
							StopBits stopBits)
{
	//QSerialPort *m_serialPort = new QSerialPort();//实例化串口类一个对象

	if (isOpen())//如果串口已经打开了 先给他关闭了
	{
		clear();
		close();
	}

	//设置串口名字 假设我们上面已经成功获取到了 并且使用第一个
	setPortName(portName);

	if (!open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
	{
		qDebug() << portName << "打开失败!";
		return false;
	}
	//打开成功

	//setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections);//设置波特率和读写方向
	//setDataBits(QSerialPort::Data8);      //数据位为8位
	//setFlowControl(QSerialPort::NoFlowControl);//无流控制
	//setParity(QSerialPort::NoParity); //无校验位
	//setStopBits(QSerialPort::OneStop); //一位停止位

	setBaudRate(baudRate,direction);//设置波特率和读写方向
	setDataBits(datBits);      //数据位为8位
	setFlowControl(flowControl);//无流控制
	setParity(parity); //无校验位
	setStopBits(stopBits); //一位停止位


	//连接信号槽 当下位机发送数据QSerialPortInfo 会发送个 readyRead 信号,我们定义个槽void receiveInfo()解析数据
	connect(this, SIGNAL(readyRead()), this, SLOT(slotDataReceived()));
	return true;
}
void QTargetSerial::slotDataReceived()
{

	QByteArray info = readAll();
	QByteArray hexData = info.toHex();
	DEBUG_PARA1("void QTargetSerial::slotDataReceived()");
	DEBUG_PARA1(hexData.data());
	if (hexData == "0x10000")
	{
		//do something
	}else if (hexData == "0x100001")
	{
		//do something
	}
	int i = 0;
	int num = info.length();
	for (i = 0; i < num; i++)
	{
		m_received_bytes.append(info.at(i));
	}

	//缓冲区的数据帧有效
	if (checkDataFrame(m_received_bytes))
	{
		DEBUG_PARA1("缓冲区的数据帧有效");
	}


	/*while (bytesAvailable())
	{
		int len = bytesAvailable();
		char buf[BUFFER_SIZE1024] = { 0 };
		read(buf, len);
		QString msg(buf);
		QByteArray ba = QByteArray::fromRawData(buf, len);
		emit sigUpdateClient(msg, len);
		emit sigUpdateClientData(ba);
	}*/
}

void QTargetSerial::slotReadData(void)
{
	qDebug() << "Reading Data...ID is:" << QThread::currentThreadId();
	clear(QSerialPort::AllDirections);
}

void QTargetSerial::slotThreadStarted(void)
{
	qDebug() << "Thread has started...ID is:" << QThread::currentThreadId();
}

void QTargetSerial::slotThreadFinished(void)
{
	qDebug() << "Closing COM port...ID is:" << QThread::currentThreadId();
	if (isOpen())
	{
		close();      // 关闭串口。
	}
}

bool  QTargetSerial::checkDataFrame(QList<char>& lstBytes)
{
	UBYTE1 buff[BUFFER_SIZE1024] = {0};
	int  len = lstBytes.length();
	int  pos = 0;
	if (len> 5)
	{
		for (pos = 0; pos < len;pos++)
		{
			buff[pos] = (UBYTE1 )lstBytes[pos];
		}

		UBYTE2 devId = modbus_functions::to_word(buff);
		UBYTE2 devType = devId >> 10;
		UBYTE2 devNo = devId & 0x03FF;
		if (devType!=1 && devType != 2)
		{
			return false;
		}

		if (devNo == 0 || devNo > 12)
		{
			return false;
		}

		UBYTE1 funcNo = (UBYTE1)buff[2];
		if (funcNo != FC_READ_ANALOG)
		{
			return false;
		}

		UBYTE2 valid_bytes = modbus_functions::to_word(&buff[3]);
		if (valid_bytes<2 || valid_bytes%2!=0 || valid_bytes>64)
		{
			return false;
		}

		if (len < valid_bytes+7)
		{
			return false;
		}

		pos = 5;
		for (pos = 5; pos < valid_bytes+5;pos+=2)
		{
			UBYTE2 val = modbus_functions::to_word(buff + pos);
		}

		UBYTE2 crc_code     = modbus_functions::to_word(buff + pos);
		UBYTE2 crc_code_calc = modbus_functions::crc->getcrc((const char*)buff, pos);
		if (crc_code != crc_code_calc)
		{
			return false;
		}

	}

	return false;
}