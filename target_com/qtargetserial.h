#ifndef QTARGETSERIAL_H
#define QTARGETSERIAL_H
#include "target_global.h"
#include "target_structs.h"
class MainWindow;
class QTargetSerial : public QSerialPort
{
	Q_OBJECT

public:
	QTargetSerial(QObject *parent, TTarget* pTarget);
	~QTargetSerial();

	static QStringList getSerialPort();
	bool openSerialPort(const QString& portName,
		BaudRate baudRate = Baud9600,
		Directions direction = AllDirections,
		DataBits datBits    = Data8,
		FlowControl flowControl=NoFlowControl,
		Parity parity=NoParity,
		StopBits stopBits=OneStop);
private:
	MainWindow*          m_mainWindow;
	TTarget*             m_target;
	QList<char>          m_received_bytes;
	bool                checkDataFrame(QList<char>& lstBytes);
signals:
	void sigUpdateClient(QString,int);
	void sigUpdateClientData(QByteArray da);
	void sigDisconnected(int);
public slots:
	void slotDisconnected();
	void slotDataReceived();
	void slotReadData(void);
	void slotThreadStarted(void);
	void slotThreadFinished(void);
};

#endif // QTARGETSERIAL_H
