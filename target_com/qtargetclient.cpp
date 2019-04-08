#include "qtargetserver.h"
#include "qtargetclient.h"
#include "target_constants.h"
QTargetClient::QTargetClient(QObject *parent, TTarget* pTarget) : QTcpSocket((QObject*)parent)
{
	m_target = pTarget;
	connect(this, SIGNAL(readyRead()),   this, SLOT(slotDataReceived()));
	connect(this, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
	connect(this, SIGNAL(connected()), this, SLOT(slotConnected()));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
	connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotStateChanged(QAbstractSocket::SocketState)));
}

QTargetClient::~QTargetClient()
{

}

void QTargetClient::slotDataReceived()
{
	if (m_target)
	{
		while (bytesAvailable())
		{
			QByteArray info = readAll();
			QByteArray hexData = info.toHex();
			DEBUG_PARA1("void QTargetClient::slotDataReceived()");
			DEBUG_PARA1(hexData.data());
			if (m_target)
			emit sigUpdateClientData(info);
		}
	}

}

void QTargetClient::slotDisconnected()
{
	if (m_target)
	{
		DEBUG_PARA2("void QTargetClient::slotDisconnected() DEVNO=",m_target->devNo);
		emit sigDisconnected(m_target);
	}

}


//连接建立
void QTargetClient::slotConnected()
{
	if (m_target)
	{
		DEBUG_PARA2("void QTargetClient::slotConnected() DEVNO=", m_target->devNo);
		emit sigConnected(m_target);
	}

}

//状态改变
void QTargetClient::slotStateChanged(QAbstractSocket::SocketState s)
{
	if (m_target)
	{
		DEBUG_PARA2("void QTargetClient::slotStateChanged() DEVNO=", m_target->devNo);
		qDebug() << "state no=" << s;
		emit sigChanged(m_target, s);
	}

}

//发生错误
void QTargetClient::slotError(QAbstractSocket::SocketError e)
{
	if (m_target)
	{
		DEBUG_PARA2("void QTargetClient::slotError() DEVNO=", m_target->devNo);
		qDebug() <<"error no="<< e;
		emit sigError(m_target, e);
	}

}