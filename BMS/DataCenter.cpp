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
		nRec = udpSocket->readDatagram(temp, length);
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
			if (FunctionNo >= 0x00 && FunctionNo <= 0x63) //�����ѹ����֡
			{
				receiveCellVol(idAndData, 11);
			}
			else if (FunctionNo >= 0x64 && FunctionNo <= 0x95)//�����¶�����֡
			{
				receiveCellTemp(idAndData, 11);
			}
			else if (FunctionNo == 0x96)//��ѹ��ֵ֡
			{
				receiveVolMaxMin(idAndData, 11);
			}
			else if (FunctionNo == 0x97)//�¶���ֵ֡
			{
				receiveTempMaxMin(idAndData, 11);
			}
			else if (FunctionNo == 0x98)//�����״̬��Ϣ
			{
				receiveBatPackStat(idAndData, 11);
			}
			else if (FunctionNo == 0x99)//��ŵ��ֹ��Ϣ֡
			{
				receiveCharDisCutOff(idAndData, 11);
			}
			else if (FunctionNo == 0x9A)//SOC��SOH��Ϣ֡
			{
				receiveSOCSOH(idAndData, 11);
			}
			else if (FunctionNo == 0xA0)
			{
				receiveBatPackConfData1(idAndData, 11);
			}
			else if (FunctionNo == 0xA1)
			{
				receiveBatPackConfData2(idAndData, 11);
			}
			else if (FunctionNo == 0xA2)
			{
				receiveBatPackConfData3(idAndData, 11);
			}
			else if (FunctionNo == 0xB0)//BMS״̬֡
			{
				receiveBMSState(idAndData, 11);
			}
			else if (FunctionNo >= 0xC0 && FunctionNo <= 0xCF)
			{
				receiveEqualStateData(idAndData, 11);
			}
			else if (FunctionNo == 0xD0)//0xD0,��غ��¶�����֡
			{
				receiveBatTempNum(idAndData, 11);
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

void DataCenter::receiveBatTempNum(quint8* pointer, int count)//��ȡ��غ��¶�����
{
	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batTempNumData.batNum = temp;//�������

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batTempNumData.tempNum = temp;//�¶�����

	emit newData(DataType::BatTempNum);
}

void DataCenter::receiveTempMaxMin(quint8* pointer, int count)
{
	tempMaxMinData.tempMax = pointer[3] - 40;//max�¶�,��40������ƫ��
	tempMaxMinData.tempMin = pointer[4] - 40;//min�¶�,��40������ƫ��
	tempMaxMinData.maxNo = pointer[5];//max�¶ȱ��
	tempMaxMinData.minNo = pointer[6];//min�¶ȱ��
	tempMaxMinData.tempDeff = pointer[7]; //�²�
	tempMaxMinData.aveTemp = pointer[8] - 40;//�����ƽ���¶�

	emit newData(DataType::TempMaxMin);
}

void DataCenter::receiveEqualStateData(quint8* pointer, int count)//����״̬
{
	equalStateData.frameNo = pointer[1] - 0xC0;

	quint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	equalStateData.equalType[0] = temp & 0x7F;
	equalStateData.equalNo[0] = temp & 0x80;

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	equalStateData.equalType[1] = temp & 0x7F;
	equalStateData.equalNo[1] = temp & 0x80;

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	equalStateData.equalType[2] = temp & 0x7F;
	equalStateData.equalNo[2] = temp & 0x80;

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	equalStateData.equalType[3] = temp & 0x7F;
	equalStateData.equalNo[3] = temp & 0x80;

	emit newData(DataType::EqualState);
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
	cellVolData.vol[0] = temp;

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	cellVolData.vol[1] = temp;

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	cellVolData.vol[2] = temp;

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	cellVolData.vol[3] = temp;

	emit newData(DataType::CellVol);
}

void DataCenter::receiveCellTemp(quint8* pointer, int count)//��ȡ�����ѹ����
{
	cellTempData.frameNo = pointer[1] = 100;
	cellTempData.temp[0] = pointer[3];
	cellTempData.temp[1] = pointer[4];
	cellTempData.temp[2] = pointer[5];
	cellTempData.temp[3] = pointer[6];
	cellTempData.temp[4] = pointer[7];
	cellTempData.temp[5] = pointer[8];
	cellTempData.temp[6] = pointer[9];
	cellTempData.temp[7] = pointer[10];
	emit newData(DataType::CellTemp);
}

//��ȡ�����������Ϣ1
void DataCenter::receiveBatPackConfData1(quint8* pointer, int count)
{
	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batPackConfData1.batType = temp;

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batPackConfData1.batDesignCapacity = (float)temp / 10;

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	batPackConfData1.bmuNum = temp;

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	batPackConfData1.cellBatNum = temp;

	emit newData(DataType::BatPackConf1);
}

//��ȡ�����������Ϣ2
void DataCenter::receiveBatPackConfData2(quint8* pointer, int count)
{
	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batPackConfData2.tempSensorNum = temp;

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batPackConfData2.endSlaveBatNum = temp;

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	batPackConfData2.endSlaveTempNum = temp;

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	batPackConfData2.batMaxCapacity = (float)temp / 10;

	emit newData(DataType::BatPackConf2);
}

//��ȡ�����������Ϣ3
void DataCenter::receiveBatPackConfData3(quint8* pointer, int count)
{
	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batPackConfData3.batCurCapacity = (float)temp / 10;

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batPackConfData3.batCurLeftCapacity = (float)temp / 10;

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	batPackConfData3.equalStartPressure = (float)temp / 10;

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	batPackConfData3.equalClosePressure = (float)temp / 10;

	emit newData(DataType::BatPackConf3);
}

void DataCenter::udpStateChanged(QAbstractSocket::SocketState socketState)
{
}

void DataCenter::udpErrorOccur(QAbstractSocket::SocketError socketError)
{
}