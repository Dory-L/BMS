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

	QUdpSocket* udpSocket;//udpClient

	//数据类型,用于通知界面更新数据
	enum DataType { BatPackStat, VolMaxMin, TempMaxMin, CharDisCutOff, SOCSOH, BMSState, CellVol, CellTemp};

	//发送数据到网络
	void sendDataToUdp(char *pointer, int count, BMS::DataFunc func);

	//状态查询
	//获取网络状态 udp不需要？
	QAbstractSocket::SocketState getUdpState() { return udpSocket->state(); }

	//获取相关信息
	BMS::BatPackStatDataSt getBatPackStatData() { return batPackStatData; }
	BMS::VolMaxMinDataSt   getVolMaxMinData() { return volMaxMinData; }
	BMS::TempMaxMinDataSt  getTempMaxMinData() { return tempMaxMinData; }
	BMS::CharDisCutOffDataSt getCharDisCutOffData() { return charDisCutOffData; }
	BMS::SOCSOHDataSt	   getSOCSOHData() { return SOCSOHData; }
	BMS::BMSStateDataSt	   getBMSStateData() { return BMSStateData; }
	BMS::CellVolDataSt	   getCellVolData() { return CellVolData; }
	BMS::CellTempDataSt	   getCellTempData() { return CellTempData; }
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
	
	
	//信号槽
	void iniConnect();

	//数据解析
	void dataAnalysisUdp(quint8 data);//网络数据解码

	//具体数据接收（根据功能号的不同会调用不同的接收函数）
	void receiveBatPackStat(quint8* pointer, int count);//读取电池组状态信息
	void receiveVolMaxMin(quint8* pointer, int count);//读取电压最值
	void receiveTempMaxMin(quint8* pointer, int count);//读取温度最值
	void receiveCharDisCutOff(quint8* pointer, int count);//读取充放电截止信息
	void receiveSOCSOH(quint8* pointer, int count);//读取SOCSOH数据
	void receiveBMSState(quint8* pointer, int count);//读取BMS状态数据
	void receiveCellVol(quint8* pointer, int count);//读取单体电压数据
	void receiveCellTemp(quint8* pointer, int count);//读取单体电压数据

	//接收的数据存储在下面的变量中，读取时直接返回这些数据
	BMS::BatPackStatDataSt	  batPackStatData;
	BMS::VolMaxMinDataSt	  volMaxMinData;
	BMS::TempMaxMinDataSt	  tempMaxMinData;
	BMS::CharDisCutOffDataSt  charDisCutOffData;
	BMS::SOCSOHDataSt		  SOCSOHData;
	BMS::BMSStateDataSt		  BMSStateData;
	BMS::CellVolDataSt		  cellVolData;
	BMS::CellTempDataSt		  cellTempData;
};
