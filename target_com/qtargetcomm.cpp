#include "qtargetcomm.h"
#include "tp_communication_if.h"
#include "target_global.h"
#include "qtargetserial.h"
#include "qtargetserver.h"
#include "qtargetclient.h"

//坐标计算
QPointF calcPoint(uchar grade, uchar direction, QPointF center, qreal radius)
{
	qreal deltGrade = radius / 10.0f;
	qreal radiusGrade = deltGrade*(10 - grade);
	qreal angle = PI / 4.0f*(direction - qrand() % 100 / 100.0f) - PI / 8.0;
	qreal deltx = radiusGrade*cos(angle);
	qreal delty = radiusGrade*sin(angle);
	QPointF deltPoint(deltx, -delty);
	QPointF retPoint = center + deltPoint;
	return retPoint;
}
//构造函数
QTargetComm::QTargetComm(QObject* parent) :QUdpSocket(parent)
{
	m_timer = NULL;
	m_status = ucsNone;
	m_current_cmd.cmdType = 0;
	m_commandList.clear();
	m_serial = NULL;
	m_tcpServer = NULL;
	memset(m_current_cmd.dataArr, 0, sizeof(m_current_cmd.dataArr));
	m_current_cmd.target = NULL;
	m_current_cmd.timeout = 1000;
	m_current_cmd.tmStamp = QDateTime::currentDateTime();
}
//析构函数
QTargetComm::~QTargetComm()
{

}

//初始化
int QTargetComm::init(ushort portNo)
{

	if (m_target_clients.count()>0)
	{
		int n = 0;
		for each (QTargetClient *client in m_target_clients)
		{
			if (client->state() != QAbstractSocket::UnconnectedState)
			{
				qDebug() << "请先将装置离线:" << client->m_target->tName << "state=" << client->state();
				qDebug() << "devNo=" << client->m_target->devNo;
				return -1;
			}
		}

		for each (QTargetClient *client in m_target_clients)
		{
			client->m_target->socketTarget = NULL;
			client->m_target = NULL;
			client->deleteLater();
		}
		m_target_clients.clear();
	}
	m_status = ucsNone;
	m_current_cmd.cmdType=0;
	m_commandList.clear();

	memset(m_current_cmd.dataArr, 0, sizeof(m_current_cmd.dataArr));
	m_current_cmd.target = NULL;
	m_current_cmd.timeout = 1000;
	m_current_cmd.tmStamp = QDateTime::currentDateTime();

	if (m_timer == NULL)
	{
		if (portNo > 0)
			bind(portNo);
		else
			bind(UDP_BIND_PORT);

		m_timer = new QTimer(this);
		m_timer->setInterval(SCAN_INTERVAL);
		connect(m_timer, &QTimer::timeout, this, &QTargetComm::slotPolling);
		connect(this, &QUdpSocket::readyRead, this, &QTargetComm::readPendingDatagrams);
		m_timer->start();
	}
	return 0;
}

//追加待发送命令
int  QTargetComm::push_command(TARGET_COMMAND& cmd)
{
	QMutexLocker _lock(&mutex);
	if (m_commandList.size()>50)
	{
		qDebug() << "命令队列已满,请稍后再试";
		return -1;
	}
	m_commandList.push_back(cmd);
	return m_commandList.size();
}

//暂停命令发送
int QTargetComm::pause(bool flag)
{
	if (flag && !(m_status & ucsPaused))
	{
		m_status += ucsPaused;
	}else if (m_status & ucsPaused)
	{
		m_status -= ucsPaused;
	}
	return 0;
}
//查询
int QTargetComm::query(TTarget* target, int cmdId)
{
	int rec = 15;
	target_query  cmd;
	tg_ctrl_cfg   cmd_ctl;
	char buff[512] = { 0 };
	memset(&cmd, 0, sizeof(target_query));
	memset(&cmd_ctl, 0, sizeof(tg_ctrl_cfg));

	cmd.targetId = target->devNo;
	cmd.userId = target->userId;
	cmd.dev_type = (dev_type_e)target->subtype;
	cmd.op_type  = cmdId;
	rec = target_query_command(&cmd,buff);

	//发送报文
	sendBuff(target, buff, rec);
	return rec;
}

//配置
int QTargetComm::config(TTarget* target, int cmdType,int *pdata)
{
	int rec = 15;
	target_query  cmd;
	tg_ctrl_cfg   cmd_ctl;
	char buff[512] = { 0 };

	memset(&cmd, 0, sizeof(target_query));
	memset(&cmd_ctl, 0, sizeof(tg_ctrl_cfg));
	memset(buff, 0, 512);

	cmd_ctl.targetId = target->devNo;//设备编号
	cmd_ctl.userId   = target->userId; // 用户授权码
	cmd_ctl.dev_type = (dev_type)target->subtype;   // DB_QD把类别
	cmd_ctl.op_type  = cmdType;       // TG_CMD_CFG_DEVNUMB;命令编号
	cmd_ctl.data     = pdata[0];
	cmd_ctl.data2    = pdata[1];
	rec=target_cfg_command(&cmd_ctl, buff);

	sendBuff(target, buff, rec);
	return rec;
}
//控制
int QTargetComm::control(TTarget* target, int cmdType, int *pdata)
{
	int rec = 15;
	target_query  cmd;
	tg_ctrl_cfg   cmd_ctl;
	char buff[512] = { 0 };

	memset(&cmd, 0, sizeof(target_query));
	memset(&cmd_ctl, 0, sizeof(tg_ctrl_cfg));
	memset(buff, 0, 512);

	cmd_ctl.targetId = target->targetNo;//设备编号
	cmd_ctl.userId = target->userId; // 用户授权码
	cmd_ctl.dev_type = (dev_type)target->subtype;   // DB_QD把类别
	cmd_ctl.op_type = cmdType;       // TG_CMD_CFG_DEVNUMB;命令编号
	cmd_ctl.data = pdata[0];
	cmd_ctl.data2 = pdata[1];
	rec = target_ctrl_command(&cmd_ctl, buff);

	sendBuff(target, buff, rec);
	return rec;
}

//超时处理
void QTargetComm::slotResponseTimeout()
{
	if (m_status & ucsWaiting)
	{
		qDebug()<<"void QTargetComm::slotResponseTimeout()";
		m_status -= ucsWaiting;
		m_current_cmd.target->m_status.online = 0;
	}
}

//客户端数据更新
void QTargetComm::slotUpdateClientData(QByteArray oDatagram)
{

	DEBUG_PARA2(" QTargetComm::buff received:",oDatagram.toHex(););
	if (m_current_cmd.target && (m_status&ucsWaiting))
	{
		m_status -= ucsWaiting;
		char* pbuff = oDatagram.data();
		int   nsize = oDatagram.size();

		target_query query;
		if (target_query_return_msg(pbuff, nsize, &query) >= 0)
		{
			if (query.op_type == TG_CMD_QUERY_STATUS)
			{
				qDebug() << QString::fromLocal8Bit("状态查询");
				//target->status = query.data.stat_info.hit
				m_current_cmd.target->battery = query.data.stat_info.battery;
				m_current_cmd.target->cycle_hits = query.data.stat_info.cycle_hits;
				m_current_cmd.target->fault = query.data.stat_info.fault;
				m_current_cmd.target->hit_total = query.data.stat_info.hit_total;
				m_current_cmd.target->m_points.clear();
				for (int i = 0; i < m_current_cmd.target->hit_total && i <= 10; i++)
				{
					uchar direction = abs(query.data.stat_info.hit[i].direction);
					uchar grade = query.data.stat_info.hit[i].grade;
					QPointF p = calcPoint(grade, direction, TARGET_CENTER, TARGET_RADIUS);
					m_current_cmd.target->m_points.push_back(p);

				}
			}
			else if (query.op_type == TG_CMD_QUERY_POSISION)
			{
				qDebug() << QString::fromLocal8Bit("坐标查询");
			}
			else if (query.op_type == TG_CMD_CTL_UPDOWN)
			{
				qDebug() << QString::fromLocal8Bit("起倒控制");
			}

			//表示在线
			m_current_cmd.target->m_status.online = 1;
			emit sigUpdateTarget(m_current_cmd.target);
		}
	}
}

//定时轮询
void QTargetComm::slotPolling()
{
	QMutexLocker _lock(&mutex);
	//qDebug() << "slotPolling thread id = " << QThread::currentThreadId();

	if (m_status & ucsWaiting)
	{
		qDebug() << "void QTargetComm::slotPolling() is busy";
		qint64 delta = m_current_cmd.tmStamp.msecsTo(QDateTime::currentDateTime());
		if (delta > m_current_cmd.timeout>10000 || delta > m_current_cmd.timeout<10)
		{
			m_current_cmd.timeout = 1000;
		}
		if (delta > m_current_cmd.timeout)
		{
			slotResponseTimeout();
		}
		return;
	}

	if (m_commandList.size()>0)
	{
		m_current_cmd = m_commandList.front();
		m_commandList.pop_front();
		int ret = 0;
		m_status += ucsWaiting;
		switch (m_current_cmd.cmdType)
		{
			case TG_CMD_QUERY_STATUS://状态查询
				ret = query(m_current_cmd.target, m_current_cmd.cmdType);
				break;
			case TG_CMD_CFG_DEVNUMB://配置设备编号
				ret = control(m_current_cmd.target, m_current_cmd.cmdType, m_current_cmd.dataArr);
				break;
			case TG_CMD_CFG_DEVCMU://配置通信模式
				ret = config(m_current_cmd.target, m_current_cmd.cmdType, m_current_cmd.dataArr);
				break;
			case TG_CMD_CFG_MODE://动作模式设置
				ret = config(m_current_cmd.target, m_current_cmd.cmdType, m_current_cmd.dataArr);
				break;
			case TG_CMD_CTL_RESET://模式重置
				ret = config(m_current_cmd.target, m_current_cmd.cmdType, m_current_cmd.dataArr);
				break;
			case TG_CMD_CTL_UPDOWN://起倒/侧转控制
				ret = config(m_current_cmd.target, m_current_cmd.cmdType, m_current_cmd.dataArr);
				break;
			case TG_CMD_COM_CONNECT://连接
				if (0 > connectTarget(m_current_cmd.target))
				{
					m_status -= ucsWaiting;
				}				
				break;
			case TG_CMD_COM_DISCONNECT://断开
				if (0 > disconnectTarget(m_current_cmd.target))
				{
					m_status -= ucsWaiting;
				}
				break;
			default:
				qDebug() << "void QTargetComm::slotPolling() cmdId is  invalid";
				return;//无效的
		}
		m_current_cmd.tmStamp = QDateTime::currentDateTime();
	}
	
}

//读取数据
void QTargetComm::readPendingDatagrams()
{
	QMutexLocker _lock(&mutex);
	qDebug() << "readPendingDatagrams thread id = " << QThread::currentThreadId();
	while (hasPendingDatagrams())
	{
		//读数据包
		QByteArray oDatagram;
		oDatagram.resize(pendingDatagramSize());
		readDatagram(oDatagram.data(), oDatagram.size());
		
		slotUpdateClientData(oDatagram);
	}
}

//测试
void QTargetComm::test(int style)
{
	TTarget target;
	target.targetNo = 5;
	target.shapeType = DB_JD_QD;
	int data[4] = { 0 };
	//状态查询
	query(&target, TG_CMD_QUERY_STATUS);


	//配置设备编号
	data[0] = 46;
	data[1] = 10;
	control(&target, TG_CMD_CFG_DEVNUMB, data);

	//配置通信模式
	data[0] = TG_CM_CYCLE_SEND;
	data[1] = 20;
	config(&target, TG_CMD_CFG_DEVCMU, data);

	//动作模式设置
	data[0] = TG_RUN_HIT_ENABLE_DOWN;//命中即到
	data[1] = 0;
	config(&target, TG_CMD_CFG_MODE, data);

	//模式重置
	data[0] = 0;
	data[1] = 0;
	config(&target, TG_CMD_CTL_RESET, data);

	//起倒/侧转控制
	data[0] = TG_RUN_TO_DOWN;//倒靶
	data[1] = 4;//速度4
	config(&target, TG_CMD_CTL_UPDOWN, data);

	//起倒/侧转控制
	data[0] = TG_RUN_TO_UP;//起靶
	data[1] = 4;//速度4
	config(&target, TG_CMD_CTL_UPDOWN, data);
	return;
}

//TCP服务端的开启与关闭
void QTargetComm::on_actionTcpServer_triggered(bool bFlag)
{
	DEBUG_PARA1("on_actionTcpServer_triggered");
	static bool bRet = false;
	if (m_tcpServer == NULL)
	{
		m_tcpServer = new QTargetServer(NULL, m_target_setting->tcp_port);
		bRet = m_tcpServer->isListening();
		connect(&tcpServerThread, SIGNAL(started()), m_tcpServer, SLOT(slotThreadStarted()), Qt::QueuedConnection);
		connect(&tcpServerThread, SIGNAL(finished()), m_tcpServer, SLOT(slotThreadFinished()), Qt::DirectConnection);
	}

	if (!bRet)
	{
		DEBUG_PARA1("tcpListening  FAILED");
		return;
	}

	if (bFlag && !tcpServerThread.isRunning())
	{
		tcpServerThread.start(QThread::HighestPriority);      // 开启线程，串口接收线程的优先级较高。
		m_tcpServer->moveToThread(&tcpServerThread);            // 将串口接受对象移动到新线程中。
	}
	else
	{
		if (tcpServerThread.isRunning())
		{
			QString strTitle(tr("询问"));
			QString strText(tr("tcp服务线程已经运行你确定关闭么?"));
			//int ret = QMessageBox::question(this, strTitle, strText, QMessageBox::Yes, QMessageBox::No);
			/*if (ret == QMessageBox::Yes)
			{
				tcpServerThread.exit();                // 结束该线程。
				tcpServerThread.wait();
				while (!tcpServerThread.isFinished())
				{
					;
				}
				DEBUG_PARA1("TCP服务线程已经关闭");
			}*/
		}
	}
}

//Socket开启与关闭
void QTargetComm::on_actionSerial_triggered(bool bFlag)
{
	DEBUG_PARA1("on_actionSerial_triggered");
	static bool bRet = false;
	if (m_serial == NULL)
	{
		m_serial = new QTargetSerial(NULL,NULL);
		connect(&serialThread, SIGNAL(started()), m_serial, SLOT(slotThreadStarted()), Qt::QueuedConnection);     // 注意，serialThread 与 serial 并不在同一个线程中，因此使用 Qt::QueuedConnection。
		connect(&serialThread, SIGNAL(finished()), m_serial, SLOT(slotThreadFinished()), Qt::DirectConnection);   // serialThread 的 finished() 信号是在新线程中执行的，因此此处要使用 Qt::DirectConnection。
		bRet = m_serial->openSerialPort(m_target_setting->com_name,
			(QSerialPort::BaudRate)m_target_setting->com_baudrate,
			(QSerialPort::Direction)m_target_setting->com_direction,
			(QSerialPort::DataBits)m_target_setting->com_databit,
			(QSerialPort::FlowControl)m_target_setting->com_flowcontrol,
			(QSerialPort::Parity)m_target_setting->com_parity,
			(QSerialPort::StopBits)m_target_setting->com_stopbit);
	}

	if (!bRet)
	{
		DEBUG_PARA1("openSerialPort FAILED");
		return;
	}

	if (bFlag && !serialThread.isRunning())
	{
		serialThread.start(QThread::HighestPriority);      // 开启线程，串口接收线程的优先级较高。
		m_serial->moveToThread(&serialThread);            // 将串口接受对象移动到新线程中。
	}
	else
	{
		if (serialThread.isRunning())
		{
			QString strTitle(tr("询问"));
			QString strText(tr("串口线程已经运行你确定关闭么?"));
			/*int ret = QMessageBox::question(this, strTitle, strText, QMessageBox::Yes, QMessageBox::No);
			if (ret == QMessageBox::Yes)
			{
				serialThread.exit();                // 结束该线程。
				serialThread.wait();
				while (!serialThread.isFinished())
				{
					;
				}
				DEBUG_PARA1("线程已经关闭");
			}*/
		}
	}
}

//报文发送
int QTargetComm::sendBuff(TTarget* target,char* buff, int len)
{
	//UDP发送
	QByteArray ba = QByteArray::fromRawData(buff, len);
	qDebug() << "IP=" << target->strIPAddr << ",PORT=" << target->portNo;
	qDebug() << "buff to send :" << ba.toHex();

	//端口2000-2999分配给UDP规约的装置,3000-4000分配给TCP
	if (target->portNo >= 2000 && target->portNo < 3000)
	{
		QHostAddress addr(target->strIPAddr);
		writeDatagram(buff, len, addr, target->portNo);
	}
	else if (target->portNo >= 3000 && target->portNo < 4000)
	{
		if (target->m_status.online)
		{
			target->socketTarget->write(buff, len);
			if (!target->socketTarget->waitForBytesWritten())
			{
				DEBUG_PARA2("发送tcp报文失败 devno=", target->portNo);
				return -2;
			}
		}
		return len;
	}
	return 0;
}


void QTargetComm::slotClientConnected(TTarget* t)
{
	DEBUG_PARA1("slotClientConnected(TTarget* t)");
	t->m_status.online = 1;
	if (m_current_cmd.target == t && m_status&ucsWaiting)
	{
		if (m_current_cmd.cmdType == TG_CMD_COM_CONNECT)
		{
			m_status -= ucsWaiting;
		}
	}
}
void QTargetComm::slotClientDisconnected(TTarget* t)
{
	DEBUG_PARA1("slotClientDisconnected(TTarget* t)");
	t->m_status.online = 0;	
	if (m_current_cmd.target == t && m_status&ucsWaiting)
	{
		if (m_current_cmd.cmdType == TG_CMD_COM_DISCONNECT)
		{
			m_status -= ucsWaiting;
		}
	}

	//tcp连接重置
	for each (QTargetClient* client  in m_target_clients)
	{
		if (client && client->m_target == t)
		{
			while (client->state() != QAbstractSocket::UnconnectedState)
			{
				QThread::msleep(100);
			}
			m_target_clients.removeOne(client);
			client->m_target->socketTarget = NULL;
			client->m_target = NULL;
			client->deleteLater();//稍后销毁,防止途中报文收到后无法处理
			break;
		}
	}
}
void QTargetComm::slotClientError(TTarget* t, SocketError e)
{
	DEBUG_PARA1("slotClientError(TTarget* t,SocketError e)");
	t->m_status.online = 0;
	if (m_current_cmd.target == t && m_status&ucsWaiting)
	{
		m_status -= ucsWaiting;
	}
}
void QTargetComm::slotClientChanged(TTarget* t, SocketState s)
{
	DEBUG_PARA1("slotClientChanged(TTarget* t,SocketState s)");
	switch (s)
	{
	case QAbstractSocket::UnconnectedState:
		t->m_status.online = 0;
		break;
	case QAbstractSocket::HostLookupState:
		break;
	case QAbstractSocket::ConnectingState:
		break;
	case QAbstractSocket::ConnectedState:
		t->m_status.online = 1;
		break;
	case QAbstractSocket::BoundState:
		break;
	case QAbstractSocket::ListeningState:
		break;
	case QAbstractSocket::ClosingState:
		break;
	default:
		break;
	}


}

int QTargetComm::connectTarget(TTarget* target)
{
	if (NULL == target->socketTarget)
	{
		target->socketTarget = new QTargetClient(this, target);
		m_target_clients.append(target->socketTarget);
		connect(target->socketTarget,
			SIGNAL(sigUpdateClientData(QByteArray)),
			this,
			SLOT(slotUpdateClientData(QByteArray)));

		connect(target->socketTarget,
			SIGNAL(sigConnected(TTarget*)),
			this,
			SLOT(slotClientConnected(TTarget*)));

		connect(target->socketTarget,
			SIGNAL(sigDisconnected(TTarget*)),
			this,
			SLOT(slotClientDisconnected(TTarget*)));

		connect(target->socketTarget,
			SIGNAL(sigChanged(TTarget*, SocketState)),
			this,
			SLOT(slotClientChanged(TTarget*,SocketState)));

		connect(target->socketTarget,
			SIGNAL(sigError(TTarget*, SocketError)),
			this,
			SLOT(slotClientError(TTarget*, SocketError)));

		target->m_status.online = 0;
	}

	if (target->m_status.online == 0)
	{
		target->socketTarget->connectToHost(target->strIPAddr, target->portNo);
		bool connected = target->socketTarget->waitForConnected();
		if (!connected)
		{
			qDebug() << __FILE__ << "," << __LINE__;
			qDebug() << "device can't be connected [" << target->strIPAddr << ":" << target->portNo << "]";
			target->m_status.online = 0;
			return -1;
		}else
		{
			target->m_status.online = 1;
		}
		
	}
	return 0;
}

int QTargetComm::disconnectTarget(TTarget* target)
{
	if (NULL == target->socketTarget)
	{
		DEBUG_PARA2("非tcp连接靶标,连接无需关闭",target->tName);
		return -1;
	}else
	{
		//发送连接关闭事件
		if (target->socketTarget->state() != QAbstractSocket::UnconnectedState)
		target->socketTarget->close();
	}

	return 0;
}