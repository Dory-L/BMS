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

	//��������,����֪ͨ�����������
	enum DataType { BatPackStat, VolMaxMin, TempMaxMin, CharDisCutOff, SOCSOH, BMSState, 
		CellVol, CellTemp, BatTempNum, EqualState, BatPackConf1, BatPackConf2, BatPackConf3};

	//�������ݵ�����
	void sendDataToUdp(char *pointer, int count, BMS::DataFunc func);

	//״̬��ѯ
	//��ȡ����״̬ udp����Ҫ��
	QAbstractSocket::SocketState getUdpState() { return udpSocket->state(); }

	//��ȡ�����Ϣ
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
signals:
	void newData(DataCenter::DataType type);
	void warning(QString str);
	void error(QString str);
	void newMassage(QString str);
public slots:

private slots :
	//���ݽ���
	void receiveUdpData();

	//IO״̬
	void udpStateChanged(QAbstractSocket::SocketState socketState);
	void udpErrorOccur(QAbstractSocket::SocketError socketError);

private:
	
	
	//�źŲ�
	void iniConnect();

	//���ݽ���
	void dataAnalysisUdp(quint8 data);//�������ݽ���

	//�������ݽ��գ����ݹ��ܺŵĲ�ͬ����ò�ͬ�Ľ��պ�����
	void receiveBatPackStat(quint8* pointer, int count);//��ȡ�����״̬��Ϣ
	void receiveVolMaxMin(quint8* pointer, int count);//��ȡ��ѹ��ֵ
	void receiveTempMaxMin(quint8* pointer, int count);//��ȡ�¶���ֵ
	void receiveCharDisCutOff(quint8* pointer, int count);//��ȡ��ŵ��ֹ��Ϣ
	void receiveSOCSOH(quint8* pointer, int count);//��ȡSOCSOH����
	void receiveBMSState(quint8* pointer, int count);//��ȡBMS״̬����
	void receiveCellVol(quint8* pointer, int count);//��ȡ�����ѹ����
	void receiveCellTemp(quint8* pointer, int count);//��ȡ�����ѹ����
	void receiveBatTempNum(quint8* pointer, int count);//��ȡ��غ��¶�����
	void receiveEqualStateData(quint8* pointer, int count);//��ȡ����״̬
	void receiveBatPackConfData1(quint8* pointer, int count);//��ȡ�����������Ϣ1
	void receiveBatPackConfData2(quint8* pointer, int count);//��ȡ�����������Ϣ2
	void receiveBatPackConfData3(quint8* pointer, int count);//��ȡ�����������Ϣ2

	//���յ����ݴ洢������ı����У���ȡʱֱ�ӷ�����Щ����
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
};
