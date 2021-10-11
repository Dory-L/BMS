#pragma once

#include <QObject>
#include <QtNetwork/QUdpSocket>
#include "bms.h"

class DataCenter : public QObject
{
	Q_OBJECT

public:
	DataCenter(QObject *parent);
	~DataCenter();

	//数据类型,用于通知界面更新数据
	enum DataType { CommSuccData, VolCalData, CurCalData, BatPackConfData, AlaProParData};

	//发送数据到网络
	void sendDataToUdp(char *pointer, int count, BMS::DataFunc func);

	//状态查询
	//获取网络状态 udp不需要？
	QAbstractSocket::SocketState getUdpState() { return udpSocket->state(); }
signals:
	void newData(DataCenter::DataType type);
	void warning(QString str);
	void error(QString str);
	void newMassage(QString str);
public slots:

private slots :
	//数据接收
	void receiveUdpData();

	//IO状态
	void udpStateChanged(QAbstractSocket::SocketState socketState);
	void udpErrorOccur(QAbstractSocket::SocketError socketError);

private:
	QUdpSocket* udpSocket;//udpClient
	
	//信号槽
	void iniConnect();

	//数据解析
	void dataAnalysisUdp(quint8 data);//网络数据解码
	//quint8 getSunCheck(quint8 *pointer, int count);//对字节求和

	//具体数据接收（根据功能号的不同会调用不同的接收函数）
	void receiveCommSucc(quint8* pointer, int count);
	void receiveVolCal(quint8* pointer, int count);
	void receiveCurCal(quint8* pointer, int count);
	void receiveBatPackConf(quint8* pointer, int count);
	void receiveAlaProPar(quint8* pointer, int count);	

	//接收的数据存储在下面的变量中，读取时直接返回这些数据
	BMS::CommSuccDataSt	      commSuccData;
	BMS::VolCalDataSt         volCalData;
	BMS::CurCalDataSt		  curCalData;
	BMS::BatPackConfDataSt	  batPackConfData;
	BMS::AlaProParDataSt	  alaProParData;
};
