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
			memcpy(idAndData, receiveBuffer, 11);
			recFlag |= OVER_FLAG;

			quint8 FunctionNo;//���ܺ�
			FunctionNo = receiveBuffer[1];

			/*-------------�ڴ˽������ݷַ�------------*/
			if (FunctionNo >= 0 || FunctionNo <= 99) //�����ѹ����֡
			{
				receiveCellVol(idAndData, 11);
			}
			else if (FunctionNo >= 100 || FunctionNo <= 149)//�����¶�����֡
			{
				receiveCellTemp(idAndData, 11);
			}
			else if (FunctionNo = 150)//��ѹ��ֵ֡
			{
				receiveVolMaxMin(idAndData, 11);
			}
			else if (FunctionNo = 151)//�¶���ֵ֡
			{
				receiveTempMaxMin(idAndData, 11);
			}
			else if (FunctionNo == 152)//�����״̬��Ϣ
			{
				receiveBatPackStat(idAndData, 11);
			}
			else if (FunctionNo = 153)//��ŵ��ֹ��Ϣ֡
			{
				receiveCharDisCutOff(idAndData, 11);
			}
			else if (FunctionNo = 154)//SOC��SOH��Ϣ֡
			{
				receiveSOCSOH(idAndData, 11);
			}
			else if (FunctionNo = 155)//BMS״̬֡
			{
				receiveBMSState(idAndData, 11);
			}

			recFlag &= ~OVER_FLAG;
		}
	}

	if (bufferIndex == BUFFER_SIZE)//��ֹ���
		bufferIndex--;
}

//���յ����״̬��Ϣ����
void DataCenter::receiveBatPackStat(quint8* pointer, int count)
{
	//quint16 temp;
	//temp = (qint16)pointer[2];
	//batPackStatData.boardNo = (float)temp / 10;//�忨�ţ�����鲻��Ҫ

	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batPackStatData.totalVol = (float)temp / 10;//�ܵ�ѹ

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batPackStatData.totalCur = (float)temp / 10;//�ܵ���

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	batPackStatData.aveVol = temp;//����ƽ����ѹ

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	batPackStatData.volDeff = temp;//����ѹ��

	emit newData(DataType::BatPackStat);
}

//��ȡ��ѹ��ֵ
void DataCenter::receiveVolMaxMin(quint8* pointer, int count)
{
	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	volMaxMinData.maxVol = temp;//max��ѹ,1LSB=1mv

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	volMaxMinData.maxNo = temp;//max��ѹ���,1LSB=1mv

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	volMaxMinData.minVol = temp;//min��ѹ,1LSB=1mv

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	volMaxMinData.minNo = temp;//min��ѹ���,1LSB=1mv

	emit newData(DataType::VolMaxMin);
}

void DataCenter::receiveTempMaxMin(quint8* pointer, int count)
{
	quint16 temp;
	
	tempMaxMinData.tempMax = pointer[3] - 40;//max�¶�,��40������ƫ��
	tempMaxMinData.tempMin = pointer[4] - 40;//min�¶�,��40������ƫ��

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	tempMaxMinData.maxNo = temp;//max�¶ȱ��

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	tempMaxMinData.minNo = temp;//min�¶ȱ��

	tempMaxMinData.tempDeff = pointer[9]; //�²�
	tempMaxMinData.aveTemp = pointer[10];//�����ƽ���¶�

	emit newData(DataType::TempMaxMin);
}

void DataCenter::receiveCharDisCutOff(quint8* pointer, int count)
{
	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	charDisCutOffData.charCutOffTotalVol = (float)temp / 10;//����ֹ�ܵ�ѹ��1LSB=0.1V

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	charDisCutOffData.disCutOffTotalVol = (float)temp / 10;//�ŵ��ֹ�ܵ�ѹ��1LSB=0.1V

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	charDisCutOffData.maxAllowedCharCur = (float)temp / 10;//��������������1LSB=0.1A

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	charDisCutOffData.maxAllowedDisCur = (float)temp / 10;//�������ŵ������1LSB=0.1A

	emit newData(DataType::CharDisCutOff);
}

void DataCenter::receiveSOCSOH(quint8* pointer, int count)
{
	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	SOCSOHData.soc = (float)temp / 10;//SOC��1LSB=0.1%

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	SOCSOHData.soh = (float)temp / 10;//�ŵ��ֹ�ܵ�ѹ��1LSB=0.1V

	emit newData(DataType::SOCSOH);
}

void DataCenter::receiveBMSState(quint8* pointer, int count)
{
	qint16 temp;
	
	BMSStateData.batState = pointer[3];//���״̬
	BMSStateData.relayState = pointer[4];//�Ӵ����ļ̵���״̬

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	BMSStateData.fetalErr = temp;//һ��������־

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	BMSStateData.warningErr = temp;//����������־

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	BMSStateData.totalVolThreshold = (float)temp / 10;//��ѹ��ֵ1LSB=0.1V

	emit newData(DataType::BMSState);
}

void DataCenter::receiveCellVol(quint8* pointer, int count)//��ȡ�����ѹ����
{
	cellVolData.frameNo = pointer[1];

	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	cellVolData.vol1 = temp;

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	cellVolData.vol2 = temp;

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	cellVolData.vol3 = temp;

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	cellVolData.vol4 = temp;

	emit newData(DataType::CellVol);
}

void DataCenter::receiveCellTemp(quint8* pointer, int count)//��ȡ�����ѹ����
{
	cellTempData.frameNo = pointer[1];
	cellTempData.temp1 = pointer[3];
	cellTempData.temp2 = pointer[4];
	cellTempData.temp3 = pointer[5];
	cellTempData.temp4 = pointer[6];
	cellTempData.temp5 = pointer[7];
	cellTempData.temp6 = pointer[8];
	cellTempData.temp7 = pointer[9];
	cellTempData.temp8 = pointer[10];
	emit newData(DataType::CellTemp);
}

void DataCenter::udpStateChanged(QAbstractSocket::SocketState socketState)
{
}

void DataCenter::udpErrorOccur(QAbstractSocket::SocketError socketError)
{
}