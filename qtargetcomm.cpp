#include "qtargetcomm.h"
#include "qbytearray.h"
#include "qstring.h"
#include "qtimer.h"
#include "tp_communication_if.h"
#include "qthread.h"

QTargetComm::QTargetComm(QObject* parent) :QUdpSocket(parent)
{
	m_timer = NULL;
	m_status = ucsNone;
	m_current_cmd.cmdType = 0;
	m_commandList.clear();

	memset(m_current_cmd.dataArr, 0, sizeof(m_current_cmd.dataArr));
	m_current_cmd.target = NULL;
	m_current_cmd.timeout = 1000;
	m_current_cmd.tmStamp = QDateTime::currentDateTime();
}

QTargetComm::~QTargetComm()
{

}


//初始化
int QTargetComm::init(ushort portNo)
{
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

	//UDP发送
	QByteArray ba = QByteArray::fromRawData(buff, rec);
	qDebug() << "IP=" << target->strIPAddr << ",PORT=" << target->portNo;
	qDebug()<<" send udp:" << ba.toHex();
	
	QHostAddress addr(target->strIPAddr);
	writeDatagram(buff, rec, addr, target->portNo);
	//writeDatagram(buff, rec, QHostAddress::Broadcast, UDP_PEER_PORT);
	
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

	//UDP发送
	QByteArray ba = QByteArray::fromRawData(buff, rec);
	qDebug() << "IP=" << target->strIPAddr << ",PORT=" << target->portNo;
	qDebug() << " send udp:" << ba.toHex();
	QHostAddress addr(target->strIPAddr);
	writeDatagram(buff, rec, addr, target->portNo);

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

	//UDP发送
	QByteArray ba = QByteArray::fromRawData(buff, rec);
	qDebug() << "IP=" << target->strIPAddr << ",PORT=" << target->portNo;
	qDebug() << " send udp:" << ba.toHex();
	QHostAddress addr(target->strIPAddr);
	writeDatagram(buff, rec, addr, target->portNo);
	//writeDatagram(buff, rec, QHostAddress::Broadcast, UDP_PEER_PORT);
	return rec;
}


void QTargetComm::slotResponseTimeout()
{
	if (m_status & ucsWaiting)
	{
		qDebug()<<"void QTargetComm::slotResponseTimeout()";
		m_status -= ucsWaiting;
		m_current_cmd.target->m_status.online = 0;
	}
}


void QTargetComm::slotPolling()
{
	QMutexLocker _lock(&mutex);
	qDebug() << "slotPolling thread id = " << QThread::currentThreadId();

	//周期轮询
	/*if (m_status & ucsPaused)
	{
		qDebug() << "void QTargetComm::slotPolling() is paused";
		return;
	}*/

	if (m_status & ucsWaiting)
	{
		qDebug() << "void QTargetComm::slotPolling() is busy";
		qint64 delta = m_current_cmd.tmStamp.msecsTo(QDateTime::currentDateTime());
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
			default:
				qDebug() << "void QTargetComm::slotPolling() cmdId is  invalid";
				return;//无效的
		}
		m_status += ucsWaiting;
		m_current_cmd.tmStamp = QDateTime::currentDateTime();
	}
	
}
QPointF calcPoint(uchar grade, uchar direction, QPointF center, qreal radius)
{
	qreal deltGrade   = radius / 10.0f;
	qreal radiusGrade = deltGrade*(10 - grade);
	qreal angle = PI / 4.0f*(direction - qrand() % 100 / 100.0f) -  PI / 8.0;
	qreal deltx = radiusGrade*cos(angle);
	qreal delty = radiusGrade*sin(angle);
	QPointF deltPoint(deltx, -delty);
	QPointF retPoint = center + deltPoint;
	return retPoint;
}
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
		qDebug() << " QSuffixScreen::readPendingDatagrams" << oDatagram.toHex();
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
					for (int i = 0; i < m_current_cmd.target->hit_total && i<=10;i++)
					{
						uchar direction  = abs(query.data.stat_info.hit[i].direction);
						uchar grade     = query.data.stat_info.hit[i].grade;
						QPointF p = calcPoint(grade, direction, TARGET_CENTER,TARGET_RADIUS);
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
}

//测试
void QTargetComm::test(int style)
{
	TTarget target;
	target.targetNo = 5;
	target.type = DB_JD_QD;
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