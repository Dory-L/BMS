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

//信号槽
void DataCenter::iniConnect()
{
	connect(this->udpSocket, &QUdpSocket::readyRead, this, &DataCenter::receiveUdpData);//有新数据到来
	connect(this->udpSocket, &QUdpSocket::stateChanged, this, &DataCenter::udpStateChanged);
	connect(this->udpSocket, static_cast<void (QUdpSocket::*)(QAbstractSocket::SocketError socketError)>
		(&QUdpSocket::error), this, &DataCenter::udpErrorOccur);
}

//发送数据到网络
void DataCenter::sendDataToUdp(char *pointer, int count, BMS::DataFunc func)
{
}

//对网络数据接收
void DataCenter::receiveUdpData()
{
	qint64 length = udpSocket->bytesAvailable();//可读数据的数量
	if (length >= 1)
	{
		//char temp[length];
		char* temp = new char[length];
		qint64 nRec = 0;
		nRec = udpSocket->readDatagram(temp, length);
		for (int i = 0; i < nRec; i++)
			dataAnalysisUdp((quint8)temp[i]);//数据解码
	}
}

//网络数据分析
void DataCenter::dataAnalysisUdp(quint8 data)
{
	static const char HEAD_FLAG = 0x80;//起始标志
	static const char OVER_FLAG = 0x40;//溢出标志
	static const int BUFFER_SIZE = 100;
	static quint8 receiveBuffer[BUFFER_SIZE];
	static char recFlag = 0;//标志变量
	static int bufferIndex = 0;//缓冲区索引

	//data前两个字节为首部，第三个字节为数据大小，第四个字节为数据功能号，首部不存入缓冲区
	if (data == 0x88)//首部1=0x88
	{
		recFlag |= HEAD_FLAG;//接收到头标志
		receiveBuffer[bufferIndex++] = data;
	}
	else if (data == 0x10)//首部2=0x10
	{
		if (recFlag&HEAD_FLAG)//完整的头帧
		{
			bufferIndex = 0;//复位缓冲区索引
			recFlag &= ~OVER_FLAG;
		}
		else
			receiveBuffer[bufferIndex++] = data;

		recFlag &= ~HEAD_FLAG;//不管是不是头标志，都要清头标志
	}
	else
	{
		receiveBuffer[bufferIndex++] = data;
		recFlag &= ~HEAD_FLAG;

		if (bufferIndex == 11)//一帧数据接收完成，id+data共11个字节
		{
			quint8 idAndData[10];//帧id和数据一起接收
			memcpy(idAndData, receiveBuffer, 11);
			recFlag |= OVER_FLAG;

			quint8 FunctionNo;//功能号
			FunctionNo = receiveBuffer[1];

			/*-------------在此进行数据分发------------*/
			if (FunctionNo >= 0x00 && FunctionNo <= 0x63) //单体电压完整帧
			{
				receiveCellVol(idAndData, 11);
			}
			else if (FunctionNo >= 0x64 && FunctionNo <= 0x95)//单体温度完整帧
			{
				receiveCellTemp(idAndData, 11);
			}
			else if (FunctionNo == 0x96)//电压最值帧
			{
				receiveVolMaxMin(idAndData, 11);
			}
			else if (FunctionNo == 0x97)//温度最值帧
			{
				receiveTempMaxMin(idAndData, 11);
			}
			else if (FunctionNo == 0x98)//电池组状态信息
			{
				receiveBatPackStat(idAndData, 11);
			}
			else if (FunctionNo == 0x99)//充放电截止信息帧
			{
				receiveCharDisCutOff(idAndData, 11);
			}
			else if (FunctionNo == 0x9A)//SOC、SOH信息帧
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
			else if (FunctionNo == 0xB0)//BMS状态帧
			{
				receiveBMSState(idAndData, 11);
			}
			else if (FunctionNo >= 0xC0 && FunctionNo <= 0xCF)
			{
				receiveEqualStateData(idAndData, 11);
			}
			else if (FunctionNo == 0xD0)//0xD0,电池和温度总数帧
			{
				receiveBatTempNum(idAndData, 11);
			}

			recFlag &= ~OVER_FLAG;
		}
	}

	if (bufferIndex == BUFFER_SIZE)//防止溢出
		bufferIndex--;
}

//接收电池组状态信息数据
void DataCenter::receiveBatPackStat(quint8* pointer, int count)
{
	//quint16 temp;
	//temp = (qint16)pointer[2];
	//batPackStatData.boardNo = (float)temp / 10;//板卡号，电池组不需要

	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batPackStatData.totalVol = (float)temp / 10;//总电压

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batPackStatData.totalCur = (float)temp / 10;//总电流

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	batPackStatData.aveVol = temp;//单体平均电压

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	batPackStatData.volDeff = temp;//单体压差

	emit newData(DataType::BatPackStat);
}

//读取电压最值
void DataCenter::receiveVolMaxMin(quint8* pointer, int count)
{
	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	volMaxMinData.maxVol = temp;//max电压,1LSB=1mv

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	volMaxMinData.maxNo = temp;//max电压编号,1LSB=1mv

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	volMaxMinData.minVol = temp;//min电压,1LSB=1mv

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	volMaxMinData.minNo = temp;//min电压编号,1LSB=1mv

	emit newData(DataType::VolMaxMin);
}

void DataCenter::receiveBatTempNum(quint8* pointer, int count)//读取电池和温度总数
{
	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batTempNumData.batNum = temp;//电池总数

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batTempNumData.tempNum = temp;//温度总数

	emit newData(DataType::BatTempNum);
}

void DataCenter::receiveTempMaxMin(quint8* pointer, int count)
{
	tempMaxMinData.tempMax = pointer[3] - 40;//max温度,有40度正向偏置
	tempMaxMinData.tempMin = pointer[4] - 40;//min温度,有40度正向偏置
	tempMaxMinData.maxNo = pointer[5];//max温度编号
	tempMaxMinData.minNo = pointer[6];//min温度编号
	tempMaxMinData.tempDeff = pointer[7]; //温差
	tempMaxMinData.aveTemp = pointer[8] - 40;//电池组平均温度

	emit newData(DataType::TempMaxMin);
}

void DataCenter::receiveEqualStateData(quint8* pointer, int count)//均衡状态
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
	charDisCutOffData.charCutOffTotalVol = (float)temp / 10;//充电截止总电压，1LSB=0.1V

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	charDisCutOffData.disCutOffTotalVol = (float)temp / 10;//放点截止总电压，1LSB=0.1V

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	charDisCutOffData.maxAllowedCharCur = (float)temp / 10;//最大允许充电电流，1LSB=0.1A

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	charDisCutOffData.maxAllowedDisCur = (float)temp / 10;//最大允许放电电流，1LSB=0.1A

	emit newData(DataType::CharDisCutOff);
}

void DataCenter::receiveSOCSOH(quint8* pointer, int count)
{
	qint16 temp;
	temp = (qint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	SOCSOHData.soc = (float)temp / 10;//SOC，1LSB=0.1%

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	SOCSOHData.soh = (float)temp / 10;//放点截止总电压，1LSB=0.1V

	emit newData(DataType::SOCSOH);
}

void DataCenter::receiveBMSState(quint8* pointer, int count)
{
	qint16 temp;
	
	BMSStateData.batState = pointer[3];//电池状态
	BMSStateData.relayState = pointer[4];//接触器的继电器状态

	temp = (qint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	BMSStateData.fetalErr = temp;//一级保护标志

	temp = (qint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	BMSStateData.warningErr = temp;//二级报警标志

	temp = (qint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	BMSStateData.totalVolThreshold = (float)temp / 10;//电压阈值1LSB=0.1V

	emit newData(DataType::BMSState);
}

void DataCenter::receiveCellVol(quint8* pointer, int count)//读取单体电压数据
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

void DataCenter::receiveCellTemp(quint8* pointer, int count)//读取单体电压数据
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

//读取电池组配置信息1
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

//读取电池组配置信息2
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

//读取电池组配置信息3
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