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

	//��������,����֪ͨ�����������
	enum DataType { CommSuccData, VolCalData, CurCalData, BatPackConfData, AlaProParData};

	//�������ݵ�����
	void sendDataToUdp(char *pointer, int count, BMS::DataFunc func);

	//״̬��ѯ
	//��ȡ����״̬ udp����Ҫ��
	QAbstractSocket::SocketState getUdpState() { return udpSocket->state(); }
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
	QUdpSocket* udpSocket;//udpClient
	
	//�źŲ�
	void iniConnect();

	//���ݽ���
	void dataAnalysisUdp(quint8 data);//�������ݽ���
	//quint8 getSunCheck(quint8 *pointer, int count);//���ֽ����

	//�������ݽ��գ����ݹ��ܺŵĲ�ͬ����ò�ͬ�Ľ��պ�����
	void receiveCommSucc(quint8* pointer, int count);
	void receiveVolCal(quint8* pointer, int count);
	void receiveCurCal(quint8* pointer, int count);
	void receiveBatPackConf(quint8* pointer, int count);
	void receiveAlaProPar(quint8* pointer, int count);	

	//���յ����ݴ洢������ı����У���ȡʱֱ�ӷ�����Щ����
	BMS::CommSuccDataSt	      commSuccData;
	BMS::VolCalDataSt         volCalData;
	BMS::CurCalDataSt		  curCalData;
	BMS::BatPackConfDataSt	  batPackConfData;
	BMS::AlaProParDataSt	  alaProParData;
};
