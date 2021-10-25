#pragma once

#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/qabstractsocket.h>
#include "bms.h"

#define BOARD_NO 0xD0//板卡号

class DataCenter : public QObject
{
	Q_OBJECT

public:
	DataCenter(QObject *parent);
	~DataCenter();

	QUdpSocket* udpSocket;//udpClient
	QHostAddress m_aimIp;
	int m_aimPort;

	//数据类型,用于通知界面更新数据
	enum DataType { BatPackStat, VolMaxMin, TempMaxMin, CharDisCutOff, SOCSOH, BMSState, 
		CellVol, CellTemp, BatTempNum, EqualState, BatPackConf1, BatPackConf2, BatPackConf3,
		BaojingPara1, BaojingPara2, BaojingPara3, BaojingPara4,
		BaohuPara1, BaohuPara2, BaohuPara3, BaohuPara4,
		HuifuPara1, HuifuPara2, HuifuPara3, HuifuPara4,
		VolCal, CurCal, EqualFunState};

	//发送数据到网络
	int sendDataToUdp(char *pointer, int count, BMS::DataFunc func);

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
	BMS::CellVolDataSt	   getCellVolData() { return cellVolData; }
	BMS::CellTempDataSt	   getCellTempData() { return cellTempData; }
	BMS::BatTempNumDataSt  getBatTempNumData() { return batTempNumData; }
	BMS::EqualStateDataSt	getEqualStateData() { return  equalStateData; }

	BMS::BatPackConfData1St	 getBatPackConfData1() { return batPackConfData1; }
	BMS::BatPackConfData2St	  getBatPackConfData2() { return batPackConfData2; }
	BMS::BatPackConfData3St	  getBatPackConfData3() { return batPackConfData3; }

	BMS::BaojingParaData1St   getBaojingParaData1() { return baojingParaData1; }
	BMS::BaojingParaData2St   getBaojingParaData2() { return baojingParaData2; }
	BMS::BaojingParaData3St   getBaojingParaData3() { return baojingParaData3; }
	BMS::BaojingParaData4St   getBaojingParaData4() { return baojingParaData4; }

	BMS::BaohuParaData1St     getBaohuParaData1() { return baohuParaData1; }
	BMS::BaohuParaData2St     getBaohuParaData2() { return baohuParaData2; }
	BMS::BaohuParaData3St     getBaohuParaData3() { return baohuParaData3; }
	BMS::BaohuParaData4St     getBaohuParaData4() { return baohuParaData4; }

	BMS::HuifuParaData1St     getHuifuParaData1() { return huifuParaData1; }
	BMS::HuifuParaData2St     getHuifuParaData2() { return huifuParaData2; }
	BMS::HuifuParaData3St     getHuifuParaData3() { return huifuParaData3; }
	BMS::HuifuParaData4St     getHuifuParaData4() { return huifuParaData4; }

	BMS::VolCalDataSt		  getVolCalData() { return volCalData; }
	BMS::CurCalDataSt		  getCurCalData() { return curCalData; }

	BMS::EqualFunStateDataSt  getEqualFunStateData() { return equalFunStateData; }
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
	void receiveBatTempNum(quint8* pointer, int count);//读取电池和温度总数
	void receiveEqualStateData(quint8* pointer, int count);//读取均衡状态

	void receiveBatPackConfData1(quint8* pointer, int count);//读取电池组配置信息1
	void receiveBatPackConfData2(quint8* pointer, int count);//读取电池组配置信息2
	void receiveBatPackConfData3(quint8* pointer, int count);//读取电池组配置信息2

	void receiveBaojingParaData1(quint8* pointer, int count);//读取报警参数1
	void receiveBaojingParaData2(quint8* pointer, int count);//读取报警参数2
	void receiveBaojingParaData3(quint8* pointer, int count);//读取报警参数3
	void receiveBaojingParaData4(quint8* pointer, int count);//读取报警参数4

	void receiveBaohuParaData1(quint8* pointer, int count);//读取保护参数1
	void receiveBaohuParaData2(quint8* pointer, int count);//读取保护参数2
	void receiveBaohuParaData3(quint8* pointer, int count);//读取保护参数3
	void receiveBaohuParaData4(quint8* pointer, int count);//读取保护参数4

	void receiveHuifuParaData1(quint8* pointer, int count);//读取恢复参数1
	void receiveHuifuParaData2(quint8* pointer, int count);//读取恢复参数2
	void receiveHuifuParaData3(quint8* pointer, int count);//读取恢复参数3
	void receiveHuifuParaData4(quint8* pointer, int count);//读取恢复参数4

	void receiveVolCal(quint8* pointer, int count);//读取电压校准参数
	void receiveCurCal(quint8* pointer, int count);//读取电流校准参数

	void receiveEqualFunState(quint8* pointer, int count);//读取均衡功能状态信息


	//接收的数据存储在下面的变量中，读取时直接返回这些数据
	BMS::BatPackStatDataSt	  batPackStatData;
	BMS::VolMaxMinDataSt	  volMaxMinData;
	BMS::TempMaxMinDataSt	  tempMaxMinData;
	BMS::CharDisCutOffDataSt  charDisCutOffData;
	BMS::SOCSOHDataSt		  SOCSOHData;
	BMS::BMSStateDataSt		  BMSStateData;
	BMS::CellVolDataSt		  cellVolData;
	BMS::CellTempDataSt		  cellTempData;
	BMS::BatTempNumDataSt	  batTempNumData;
	BMS::EqualStateDataSt	  equalStateData;

	BMS::BatPackConfData1St	  batPackConfData1;
	BMS::BatPackConfData2St	  batPackConfData2;
	BMS::BatPackConfData3St	  batPackConfData3;

	BMS::BaojingParaData1St   baojingParaData1;
	BMS::BaojingParaData2St   baojingParaData2;
	BMS::BaojingParaData3St   baojingParaData3;
	BMS::BaojingParaData4St   baojingParaData4;

	BMS::BaohuParaData1St     baohuParaData1;
	BMS::BaohuParaData2St     baohuParaData2;
	BMS::BaohuParaData3St     baohuParaData3;
	BMS::BaohuParaData4St     baohuParaData4;

	BMS::HuifuParaData1St     huifuParaData1;
	BMS::HuifuParaData2St     huifuParaData2;
	BMS::HuifuParaData3St     huifuParaData3;
	BMS::HuifuParaData4St     huifuParaData4;

	BMS::VolCalDataSt		  volCalData;
	BMS::CurCalDataSt		  curCalData;
	BMS::EqualFunStateDataSt  equalFunStateData;
};
