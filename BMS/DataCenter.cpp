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
int DataCenter::sendDataToUdp(char *pointer, int count, BMS::DataFunc func)
{
	int size = count + 5;
	char* temp = new char[size];
	temp[0] = 0x88;//���ֽ�1
	temp[1] = 0x10;//���ֽ�2
	temp[2] = 0xFC;//pf�̶�
	temp[3] = func;
	temp[4] = BOARD_NO;//�忨��
	
	memcpy(temp + 5, pointer, count);
	int ret = udpSocket->writeDatagram(temp, size, m_aimIp, m_aimPort);
	return ret;
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
			quint8 idAndData[11];//֡id������һ�����
			memcpy(idAndData, receiveBuffer, 11);
			recFlag |= OVER_FLAG;

			quint8 FunctionNo;//���ܺ�
			FunctionNo = receiveBuffer[1];

			/*-------------�ڴ˽������ݷַ�------------*/

			switch (FunctionNo)
			{
			case BMS::VolMaxMinFunc://��ѹ��ֵ֡
				receiveVolMaxMin(idAndData, 11);
				break;
			case BMS::TempMaxMinFunc://�¶���ֵ֡
				receiveTempMaxMin(idAndData, 11);
				break;
			case BMS::BatPackStatFunc://�����״̬��Ϣ
				receiveBatPackStat(idAndData, 11);
				break;
			case BMS::CharDisCutOffFunc://��ŵ��ֹ��Ϣ֡
				receiveCharDisCutOff(idAndData, 11);
				break;
			case BMS::SOCSOHFunc://SOC��SOH��Ϣ֡
				receiveSOCSOH(idAndData, 11);
				break;
			case BMS::BMSStateFunc://BMS״̬��Ϣ
				receiveBMSState(idAndData, 11);
				break;
			case BMS::BatPackConf1Func://�����������Ϣ֡1
				receiveBatPackConfData1(idAndData, 11);
				break;
			case BMS::BatPackConf2Func://�����������Ϣ֡2
				receiveBatPackConfData2(idAndData, 11);
				break;
			case BMS::BatPackConf3Func://�����������Ϣ֡3
				receiveBatPackConfData3(idAndData, 11);
				break;
			case BMS::BaojingPara1Func://������ֵ֡1
				receiveBaojingParaData1(idAndData, 11);
				break;
			case BMS::BaojingPara2Func://������ֵ֡2
				receiveBaojingParaData2(idAndData, 11);
				break;
			case BMS::BaojingPara3Func://������ֵ֡3
				receiveBaojingParaData3(idAndData, 11);
				break;
			case BMS::BaojingPara4Func://������ֵ֡4
				receiveBaojingParaData4(idAndData, 11);
				break;
			case BMS::BaohuPara1Func://������ֵ֡1
				receiveBaohuParaData1(idAndData, 11);
				break;
			case BMS::BaohuPara2Func://������ֵ֡2
				receiveBaohuParaData2(idAndData, 11);
				break;
			case BMS::BaohuPara3Func://������ֵ֡3
				receiveBaohuParaData3(idAndData, 11);
				break;
			case BMS::BaohuPara4Func://������ֵ֡4
				receiveBaohuParaData4(idAndData, 11);
				break;
			case BMS::HuifuPara1Func://�ָ���ֵ֡1
				receiveHuifuParaData1(idAndData, 11);
				break;
			case BMS::HuifuPara2Func://�ָ���ֵ֡2
				receiveHuifuParaData2(idAndData, 11);
				break;
			case BMS::HuifuPara3Func://�ָ���ֵ֡3
				receiveHuifuParaData3(idAndData, 11);
				break;
			case BMS::HuifuPara4Func://�ָ���ֵ֡4
				receiveHuifuParaData4(idAndData, 11);
				break;
			case BMS::VolCalFunc://��ѹУ׼����֡
				receiveVolCal(idAndData, 11);
				break;
			case BMS::CurCalFunc://����У׼����֡
				receiveCurCal(idAndData, 11);
				break;
			case BMS::EqualFunStateFunc://���⹦��״̬֡
				receiveEqualFunState(idAndData, 1);
				break;
			default:
				if (FunctionNo >= 0x00 && FunctionNo <= 0x63) //�����ѹ����֡
					receiveCellVol(idAndData, 11);
				else if (FunctionNo >= 0x64 && FunctionNo <= 0x95)//�����¶�����֡
					receiveCellTemp(idAndData, 11);
				else if (FunctionNo >= 0xC0 && FunctionNo <= 0xCF)//����״̬֡
					receiveEqualStateData(idAndData, 11);
				break;
			}

			//if (FunctionNo >= 0x00 && FunctionNo <= 0x63) //�����ѹ����֡
			//{
			//	receiveCellVol(idAndData, 11);
			//}
			//else if (FunctionNo >= 0x64 && FunctionNo <= 0x95)//�����¶�����֡
			//{
			//	receiveCellTemp(idAndData, 11);
			//}
			//else if (FunctionNo == 0x96)//��ѹ��ֵ֡
			//{
			//	receiveVolMaxMin(idAndData, 11);
			//}
			//else if (FunctionNo == 0x97)//�¶���ֵ֡
			//{
			//	receiveTempMaxMin(idAndData, 11);
			//}
			//else if (FunctionNo == 0x98)//�����״̬��Ϣ
			//{
			//	receiveBatPackStat(idAndData, 11);
			//}
			//else if (FunctionNo == 0x99)//��ŵ��ֹ��Ϣ֡
			//{
			//	receiveCharDisCutOff(idAndData, 11);
			//}
			//else if (FunctionNo == 0x9A)//SOC��SOH��Ϣ֡
			//{
			//	receiveSOCSOH(idAndData, 11);
			//}
			//else if (FunctionNo == 0x9B)//BMS״̬��Ϣ
			//{
			//	receiveBMSState(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA0)//�����������Ϣ֡1
			//{
			//	receiveBatPackConfData1(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA1)//�����������Ϣ֡2
			//{
			//	receiveBatPackConfData2(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA2)//�����������Ϣ֡3
			//{
			//	receiveBatPackConfData3(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA3)//������ֵ֡1
			//{
			//	receiveBaojingParaData1(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA4)//������ֵ֡2
			//{
			//	receiveBaojingParaData2(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA5)//������ֵ֡3
			//{
			//	receiveBaojingParaData3(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA6)//������ֵ֡4
			//{
			//	receiveBaojingParaData4(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA7)//������ֵ֡1
			//{
			//	receiveBaohuParaData1(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA8)//������ֵ֡2
			//{
			//	receiveBaohuParaData2(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA9)//������ֵ֡3
			//{
			//	receiveBaohuParaData3(idAndData, 11);
			//}
			//else if (FunctionNo == 0xAA)//������ֵ֡4
			//{
			//	receiveBaohuParaData4(idAndData, 11);
			//}
			//else if (FunctionNo == 0xAB)//�ָ���ֵ֡1
			//{
			//	receiveHuifuParaData1(idAndData, 11);
			//}
			//else if (FunctionNo == 0xAC)//�ָ���ֵ֡2
			//{
			//	receiveHuifuParaData2(idAndData, 11);
			//}
			//else if (FunctionNo == 0xAD)//�ָ���ֵ֡3
			//{
			//	receiveHuifuParaData3(idAndData, 11);
			//}
			//else if (FunctionNo == 0xAE)//�ָ���ֵ֡4
			//{
			//	receiveHuifuParaData4(idAndData, 11);
			//}
			//else if (FunctionNo == 0xB8)//��ѹУ׼����֡
			//{
			//	receiveVolCal(idAndData, 11);
			//}
			//else if (FunctionNo == 0xB9)//����У׼����֡
			//{
			//	receiveCurCal(idAndData, 11);
			//}
			//else if (FunctionNo == 0xBA)
			//{
			//	receiveEqualFunState(idAndData, 1);
			//}
			//else if (FunctionNo >= 0xC0 && FunctionNo <= 0xCF)//����״̬֡
			//{
			//	receiveEqualStateData(idAndData, 11);
			//}

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
	//temp = (quint16)pointer[2];
	//batPackStatData.boardNo = (float)temp / 10;//�忨�ţ�����鲻��Ҫ

	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batPackStatData.totalVol = (float)temp / 10;//�ܵ�ѹ

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batPackStatData.totalCur = (float)temp / 10;//�ܵ���

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	batPackStatData.aveVol = temp;//����ƽ����ѹ

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	batPackStatData.volDeff = temp;//����ѹ��

	emit newData(DataType::BatPackStat);
}

//��ȡ��ѹ��ֵ
void DataCenter::receiveVolMaxMin(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	volMaxMinData.maxVol = temp;//max��ѹ,1LSB=1mv

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	volMaxMinData.maxNo = temp;//max��ѹ���,1LSB=1mv

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	volMaxMinData.minVol = temp;//min��ѹ,1LSB=1mv

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	volMaxMinData.minNo = temp;//min��ѹ���,1LSB=1mv

	emit newData(DataType::VolMaxMin);
}

void DataCenter::receiveBatTempNum(quint8* pointer, int count)//��ȡ��غ��¶�����
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batTempNumData.batNum = temp;//�������

	temp = (quint16)pointer[5];
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
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	equalStateData.equalType[0] = temp & 0x8000;
	equalStateData.equalNo[0] = temp & 0x7fff;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	equalStateData.equalType[1] = temp & 0x8000;
	equalStateData.equalNo[1] = temp & 0x7fff;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	equalStateData.equalType[2] = temp & 0x8000;
	equalStateData.equalNo[2] = temp & 0x7fff;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	equalStateData.equalType[3] = temp & 0x8000;
	equalStateData.equalNo[3] = temp & 0x7fff;

	emit newData(DataType::EqualState);
}

void DataCenter::receiveCharDisCutOff(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	charDisCutOffData.charCutOffTotalVol = (float)temp / 10;//����ֹ�ܵ�ѹ��1LSB=0.1V

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	charDisCutOffData.disCutOffTotalVol = (float)temp / 10;//�ŵ��ֹ�ܵ�ѹ��1LSB=0.1V

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	charDisCutOffData.maxAllowedCharCur = (float)temp / 10;//��������������1LSB=0.1A

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	charDisCutOffData.maxAllowedDisCur = (float)temp / 10;//�������ŵ������1LSB=0.1A

	emit newData(DataType::CharDisCutOff);
}

void DataCenter::receiveSOCSOH(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	SOCSOHData.soc = (float)temp / 10;//SOC��1LSB=0.1%

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	SOCSOHData.soh = (float)temp / 10;//�ŵ��ֹ�ܵ�ѹ��1LSB=0.1V

	emit newData(DataType::SOCSOH);
}

void DataCenter::receiveBMSState(quint8* pointer, int count)
{
	quint16 temp;
	
	BMSStateData.batState = pointer[3];//���״̬

	temp = (quint16)pointer[4];
	temp <<= 8;
	temp |= (quint16)pointer[5];
	BMSStateData.relayState = temp;//�Ӵ����ļ̵���״̬

	temp = (quint16)pointer[6];
	temp <<= 8;
	temp |= (quint16)pointer[7];
	BMSStateData.fetalErr = temp;//һ��������־

	temp = (quint16)pointer[8];
	temp <<= 8;
	temp |= (quint16)pointer[9];
	BMSStateData.warningErr = temp;//����������־

	emit newData(DataType::BMSState);
}

void DataCenter::receiveCellVol(quint8* pointer, int count)//��ȡ�����ѹ����
{
	cellVolData.frameNo = pointer[1];

	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	cellVolData.vol[0] = temp;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	cellVolData.vol[1] = temp;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	cellVolData.vol[2] = temp;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	cellVolData.vol[3] = temp;

	emit newData(DataType::CellVol);
}

void DataCenter::receiveCellTemp(quint8* pointer, int count)//��ȡ�����ѹ����
{
	cellTempData.frameNo = pointer[1] - 100;
	cellTempData.temp[0] = pointer[3] - 40;
	cellTempData.temp[1] = pointer[4] - 40;
	cellTempData.temp[2] = pointer[5] - 40;
	cellTempData.temp[3] = pointer[6] - 40;
	cellTempData.temp[4] = pointer[7] - 40;
	cellTempData.temp[5] = pointer[8] - 40;
	cellTempData.temp[6] = pointer[9] - 40;
	cellTempData.temp[7] = pointer[10] - 40;
	emit newData(DataType::CellTemp);
}

//��ȡ�����������Ϣ1
void DataCenter::receiveBatPackConfData1(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batPackConfData1.batType = temp;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batPackConfData1.batDesignCapacity = (float)temp / 10;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	batPackConfData1.bmuNum = temp;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	batPackConfData1.cellBatNum = temp;

	emit newData(DataType::BatPackConf1);
}

//��ȡ�����������Ϣ2
void DataCenter::receiveBatPackConfData2(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batPackConfData2.tempSensorNum = temp;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batPackConfData2.endSlaveBatNum = temp;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	batPackConfData2.endSlaveTempNum = temp;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	batPackConfData2.batMaxCapacity = (float)temp / 10;

	emit newData(DataType::BatPackConf2);
}

//��ȡ�����������Ϣ3
void DataCenter::receiveBatPackConfData3(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batPackConfData3.batCurCapacity = (float)temp / 10;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batPackConfData3.batCurLeftCapacity = (float)temp / 10;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	batPackConfData3.tempBMUNum = temp;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	batPackConfData3.equalStartPressure = (float)temp / 10;

	emit newData(DataType::BatPackConf3);
}

//��ȡ��������1
void DataCenter::receiveBaojingParaData1(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	baojingParaData1.cellUV = temp;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	baojingParaData1.cellOV = temp;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	baojingParaData1.packUV = (float)temp / 10;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	baojingParaData1.packOV = (float)temp / 10;

	emit newData(DataType::BaojingPara1);
}
//��ȡ��������2
void DataCenter::receiveBaojingParaData2(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	baojingParaData2.charUT = temp - 40;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	baojingParaData2.charOT = temp - 40;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	baojingParaData2.dischUT = temp - 40;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	baojingParaData2.dischOT = temp - 40;

	emit newData(DataType::BaojingPara2);
}
//��ȡ��������3
void DataCenter::receiveBaojingParaData3(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	baojingParaData3.charOC = (float)temp / 10;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	baojingParaData3.dischOC = (float)temp / 10;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	baojingParaData3.commOT = temp;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	baojingParaData3.insLow = temp;

	emit newData(DataType::BaojingPara3);
}
//��ȡ��������4
void DataCenter::receiveBaojingParaData4(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	baojingParaData4.volDiff = temp;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	baojingParaData4.tempDiff = temp;
	
	emit newData(DataType::BaojingPara4);
}
//��ȡ��������1
void DataCenter::receiveBaohuParaData1(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	baohuParaData1.cellUV = temp;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	baohuParaData1.cellOV = temp;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	baohuParaData1.packUV = (float)temp / 10;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	baohuParaData1.packOV = (float)temp / 10;

	emit newData(DataType::BaohuPara1);
}
//��ȡ��������2
void DataCenter::receiveBaohuParaData2(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	baohuParaData2.charUT = temp - 40;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	baohuParaData2.charOT = temp - 40;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	baohuParaData2.dischUT = temp - 40;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	baohuParaData2.dischOT = temp - 40;

	emit newData(DataType::BaohuPara2);
}
//��ȡ��������3
void DataCenter::receiveBaohuParaData3(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	baohuParaData3.charOC = (float)temp / 10;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	baohuParaData3.dischOC = (float)temp / 10;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	baohuParaData3.commOT = temp;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	baohuParaData3.insLow = temp;

	emit newData(DataType::BaohuPara3);
}
//��ȡ��������4
void DataCenter::receiveBaohuParaData4(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	baohuParaData4.volDiff = temp;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	baohuParaData4.tempDiff = temp;

	emit newData(DataType::BaohuPara4);
}

//��ȡ�ָ�����1
void DataCenter::receiveHuifuParaData1(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	huifuParaData1.cellUV = temp;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	huifuParaData1.cellOV = temp;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	huifuParaData1.packUV = (float)temp / 10;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	huifuParaData1.packOV = (float)temp / 10;

	emit newData(DataType::HuifuPara1);
}

//��ȡ�ָ�����2
void DataCenter::receiveHuifuParaData2(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	huifuParaData2.charUT = temp - 40;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	huifuParaData2.charOT = temp - 40;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	huifuParaData2.dischUT = temp - 40;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	huifuParaData2.dischOT = temp - 40;

	emit newData(DataType::HuifuPara2);
}
//��ȡ�ָ�����3
void DataCenter::receiveHuifuParaData3(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	huifuParaData3.charOC = (float)temp / 10;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	huifuParaData3.dischOC = (float)temp / 10;

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	huifuParaData3.commOT = temp;

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	huifuParaData3.insLow = temp;

	emit newData(DataType::HuifuPara3);
}
//��ȡ�ָ�����4
void DataCenter::receiveHuifuParaData4(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	huifuParaData4.volDiff = temp;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	huifuParaData4.tempDiff = temp;

	emit newData(DataType::HuifuPara4);
}

//��ȡ��ѹУ׼����
void DataCenter::receiveVolCal(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	volCalData.offSet = temp;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	volCalData.gain = (float)temp / 1000;

	emit newData(DataType::VolCal);
}

//��ȡ����У׼����
void DataCenter::receiveCurCal(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	curCalData.offSet = temp;

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	curCalData.gain = (float)temp / 1000;

	emit newData(DataType::CurCal);
}

//��ȡ���⹦��״̬��Ϣ
void DataCenter::receiveEqualFunState(quint8* pointer, int count)
{
	equalFunStateData.state = pointer[3];
	emit newData(DataType::EqualFunState);
}
void DataCenter::udpStateChanged(QAbstractSocket::SocketState socketState)
{
}

void DataCenter::udpErrorOccur(QAbstractSocket::SocketError socketError)
{
}