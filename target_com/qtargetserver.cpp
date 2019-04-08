#include "qtargetserver.h"
#include "qtargetclient.h"
QTargetServer::QTargetServer(QWidget *parent,int port): QTcpServer((QObject*)parent)
{
	m_mainWindow = NULL;
	connect(this, SIGNAL(newConnection()), this, SLOT(slotConnection()));
	listen(QHostAddress::Any, port);

}

QTargetServer::~QTargetServer()
{

}
void QTargetServer::slotUpdateClient(QString msg, int len)
{
	emit updateServer(msg, len);
	for each(QTargetClient* client in m_Clients)
	{
		if (client->write(msg.toLatin1(),len) != len)
		{
			continue;
		}
	}
}
void QTargetServer::slotUpdateClientData(QByteArray dat)
{
	DEBUG_PARA1("slotUpdateClientData");
}
void QTargetServer::slotDisconnected(int client_id)
{
	DEBUG_PARA1("slotDisconnected");
	for each(QTargetClient* client in m_Clients)
	{
		if (client->socketDescriptor() == client_id)
		{
			m_Clients.removeOne(client);
		}
	}
}


void QTargetServer::incomingConnection(qintptr handle)
{
	QTargetClient* client = new QTargetClient(this, NULL);
	qDebug() << "incomingConnection socketDescriptor:" << handle;

	connect(client, SIGNAL(sigUpdateClient(QString, int)), this, SLOT(slotUpdateClient(QString, int)));
	connect(client, SIGNAL(sigDisconnected(int)), this, SLOT(slotDisconnected(int)));
	m_Clients.append(client);
	client->setSocketDescriptor(handle);

}

void QTargetServer::slotThreadStarted(void)
{
	qDebug() << "tcp Thread has started...ID is:" << QThread::currentThreadId();
}

void QTargetServer::slotThreadFinished(void)
{
	qDebug() << "Closing tcp server...ID is:" << QThread::currentThreadId();
	close();
}