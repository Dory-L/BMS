#include "DataCenter.h"

DataCenter::DataCenter(QObject *parent)
	: QObject(parent)
{
	udpSocket = new QUdpSocket(this);
	iniConnect();
}

DataCenter::~DataCenter()
{
	udpSocket->close();
	delete udpSocket;
}

//�źŲ�
void DataCenter::iniConnect()
{
	connect(this->udpSocket, &QUdpSocket::readyRead, this, &DataCenter::receiveUdpData);//�������ݵ���
	connect(this->udpSocket, &QUdpSocket::stateChanged, this, &DataCenter::udpStateChanged);
	connect(this->udpSocket, static_cast<void (QUdpSocket::*)(QAbstractSocket::SocketError socketError)>
		(&QUdpSocket::error), this, &DataCenter::udpErrorOccur);
}

//�������ݵ�����
void DataCenter::sendDataToUdp(char *pointer, int count, BMS::DataFunc func)
{
}

//���������ݽ���
void DataCenter::receiveUdpData()
{
	qint64 length = udpSocket->bytesAvailable();//�ɶ����ݵ�����
	if (length >= 1)
	{
		//char temp[length];
		char* temp = new char[length];
		qint64 nRec = 0;
		nRec = udpSocket->read(temp, length);
		for (int i = 0; i < nRec; i++)
			dataAnalysisUdp((quint8)temp[i]);//���ݽ���
	}
}

//�������ݷ���
void DataCenter::dataAnalysisUdp(quint8 data)
{
	static const char HEAD_FLAG = 0x80;//��ʼ��־
	static const char OVER_FLAG = 0x40;//�����־
	static const int BUFFER_SIZE = 100;
	static quint8 receiveBuffer[BUFFER_SIZE];
	static char recFlag = 0;//��־����
	static int bufferIndex = 0;//����������

	//dataǰ�����ֽ�Ϊ�ײ����������ֽ�Ϊ���ݴ�С�����ĸ��ֽ�Ϊ���ݹ��ܺţ��ײ������뻺����
	if (data == 0x88)//�ײ�1=0x88
	{
		recFlag |= HEAD_FLAG;//���յ�ͷ��־
		receiveBuffer[bufferIndex++] = data;
	}
	else if (data == 0x10)//�ײ�2=0x10
	{
		if (recFlag&HEAD_FLAG)//������ͷ֡
		{
			bufferIndex = 0;//��λ����������
			recFlag &= ~OVER_FLAG;
		}
		else
			receiveBuffer[bufferIndex++] = data;

		recFlag &= ~HEAD_FLAG;//�����ǲ���ͷ��־����Ҫ��ͷ��־
	}
	else
	{
		receiveBuffer[bufferIndex++] = data;
		recFlag &= ~HEAD_FLAG;

		if (bufferIndex == 11)//һ֡���ݽ�����ɣ�id+data��11���ֽ�
		{
			quint8 idAndData[10];//֡id������һ�����
			memcpy(idAndData, receiveBuffer + 3, 10);
			recFlag |= OVER_FLAG;
			/*--------*/  switch (receiveBuffer[1])/*--------�ڴ˽������ݷַ�------------*/
			{
			case (quint8)BMS::CommSuccFunc:
				receiveCommSucc(idAndData, 10);
				break;
			case (quint8)BMS::VolCalFunc:
				receiveVolCal(idAndData, 10);
				break;
			case (quint8)BMS::CurCalFunc://GPS��Ϣ
				receiveCurCal(idAndData, 10);
				break;
			case (quint8)BMS::BatPackConfFunc://DHT22��Ϣ
				receiveBatPackConf(idAndData, 10);
				break;
			case (quint8)BMS::AlaProParFunc://AHRS��Ϣ
				receiveAlaProPar(idAndData, 10);
				break;
			defalut://δ���幦�ܺ�
				emit warning("tcpSocket���յ�δ�������ݸ�ʽ��.");
				break;
			}

			recFlag &= ~OVER_FLAG;
		}
	}

	if (bufferIndex == BUFFER_SIZE)//��ֹ���
		bufferIndex--;
}

//����ͨ�ųɹ�ָ������
void DataCenter::receiveCommSucc(quint8* pointer, int count)
{
}
//���յ�ѹУ׼����
void DataCenter::receiveVolCal(quint8* pointer, int count)
{
}
//���յ���У׼����
void DataCenter::receiveCurCal(quint8* pointer, int count)
{
}
//���յ������������
void DataCenter::receiveBatPackConf(quint8* pointer, int count)
{
}
//���ձ���&������������
void DataCenter::receiveAlaProPar(quint8* pointer, int count)
{
}

void DataCenter::udpStateChanged(QAbstractSocket::SocketState socketState)
{
}

void DataCenter::udpErrorOccur(QAbstractSocket::SocketError socketError)
{
}