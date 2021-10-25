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
int DataCenter::sendDataToUdp(char *pointer, int count, BMS::DataFunc func)
{
	int size = count + 5;
	char* temp = new char[size];
	temp[0] = 0x88;//首字节1
	temp[1] = 0x10;//首字节2
	temp[2] = 0xFC;//pf固定
	temp[3] = func;
	temp[4] = BOARD_NO;//板卡号
	
	memcpy(temp + 5, pointer, count);
	int ret = udpSocket->writeDatagram(temp, size, m_aimIp, m_aimPort);
	return ret;
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
			quint8 idAndData[11];//帧id和数据一起接收
			memcpy(idAndData, receiveBuffer, 11);
			recFlag |= OVER_FLAG;

			quint8 FunctionNo;//功能号
			FunctionNo = receiveBuffer[1];

			/*-------------在此进行数据分发------------*/

			switch (FunctionNo)
			{
			case BMS::VolMaxMinFunc://电压最值帧
				receiveVolMaxMin(idAndData, 11);
				break;
			case BMS::TempMaxMinFunc://温度最值帧
				receiveTempMaxMin(idAndData, 11);
				break;
			case BMS::BatPackStatFunc://电池组状态信息
				receiveBatPackStat(idAndData, 11);
				break;
			case BMS::CharDisCutOffFunc://充放电截止信息帧
				receiveCharDisCutOff(idAndData, 11);
				break;
			case BMS::SOCSOHFunc://SOC、SOH信息帧
				receiveSOCSOH(idAndData, 11);
				break;
			case BMS::BMSStateFunc://BMS状态信息
				receiveBMSState(idAndData, 11);
				break;
			case BMS::BatPackConf1Func://电池组配置信息帧1
				receiveBatPackConfData1(idAndData, 11);
				break;
			case BMS::BatPackConf2Func://电池组配置信息帧2
				receiveBatPackConfData2(idAndData, 11);
				break;
			case BMS::BatPackConf3Func://电池组配置信息帧3
				receiveBatPackConfData3(idAndData, 11);
				break;
			case BMS::BaojingPara1Func://报警阈值帧1
				receiveBaojingParaData1(idAndData, 11);
				break;
			case BMS::BaojingPara2Func://报警阈值帧2
				receiveBaojingParaData2(idAndData, 11);
				break;
			case BMS::BaojingPara3Func://报警阈值帧3
				receiveBaojingParaData3(idAndData, 11);
				break;
			case BMS::BaojingPara4Func://报警阈值帧4
				receiveBaojingParaData4(idAndData, 11);
				break;
			case BMS::BaohuPara1Func://保护阈值帧1
				receiveBaohuParaData1(idAndData, 11);
				break;
			case BMS::BaohuPara2Func://保护阈值帧2
				receiveBaohuParaData2(idAndData, 11);
				break;
			case BMS::BaohuPara3Func://保护阈值帧3
				receiveBaohuParaData3(idAndData, 11);
				break;
			case BMS::BaohuPara4Func://保护阈值帧4
				receiveBaohuParaData4(idAndData, 11);
				break;
			case BMS::HuifuPara1Func://恢复阈值帧1
				receiveHuifuParaData1(idAndData, 11);
				break;
			case BMS::HuifuPara2Func://恢复阈值帧2
				receiveHuifuParaData2(idAndData, 11);
				break;
			case BMS::HuifuPara3Func://恢复阈值帧3
				receiveHuifuParaData3(idAndData, 11);
				break;
			case BMS::HuifuPara4Func://恢复阈值帧4
				receiveHuifuParaData4(idAndData, 11);
				break;
			case BMS::VolCalFunc://电压校准参数帧
				receiveVolCal(idAndData, 11);
				break;
			case BMS::CurCalFunc://电流校准参数帧
				receiveCurCal(idAndData, 11);
				break;
			case BMS::EqualFunStateFunc://均衡功能状态帧
				receiveEqualFunState(idAndData, 1);
				break;
			default:
				if (FunctionNo >= 0x00 && FunctionNo <= 0x63) //单体电压完整帧
					receiveCellVol(idAndData, 11);
				else if (FunctionNo >= 0x64 && FunctionNo <= 0x95)//单体温度完整帧
					receiveCellTemp(idAndData, 11);
				else if (FunctionNo >= 0xC0 && FunctionNo <= 0xCF)//均衡状态帧
					receiveEqualStateData(idAndData, 11);
				break;
			}

			//if (FunctionNo >= 0x00 && FunctionNo <= 0x63) //单体电压完整帧
			//{
			//	receiveCellVol(idAndData, 11);
			//}
			//else if (FunctionNo >= 0x64 && FunctionNo <= 0x95)//单体温度完整帧
			//{
			//	receiveCellTemp(idAndData, 11);
			//}
			//else if (FunctionNo == 0x96)//电压最值帧
			//{
			//	receiveVolMaxMin(idAndData, 11);
			//}
			//else if (FunctionNo == 0x97)//温度最值帧
			//{
			//	receiveTempMaxMin(idAndData, 11);
			//}
			//else if (FunctionNo == 0x98)//电池组状态信息
			//{
			//	receiveBatPackStat(idAndData, 11);
			//}
			//else if (FunctionNo == 0x99)//充放电截止信息帧
			//{
			//	receiveCharDisCutOff(idAndData, 11);
			//}
			//else if (FunctionNo == 0x9A)//SOC、SOH信息帧
			//{
			//	receiveSOCSOH(idAndData, 11);
			//}
			//else if (FunctionNo == 0x9B)//BMS状态信息
			//{
			//	receiveBMSState(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA0)//电池组配置信息帧1
			//{
			//	receiveBatPackConfData1(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA1)//电池组配置信息帧2
			//{
			//	receiveBatPackConfData2(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA2)//电池组配置信息帧3
			//{
			//	receiveBatPackConfData3(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA3)//报警阈值帧1
			//{
			//	receiveBaojingParaData1(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA4)//报警阈值帧2
			//{
			//	receiveBaojingParaData2(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA5)//报警阈值帧3
			//{
			//	receiveBaojingParaData3(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA6)//报警阈值帧4
			//{
			//	receiveBaojingParaData4(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA7)//保护阈值帧1
			//{
			//	receiveBaohuParaData1(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA8)//保护阈值帧2
			//{
			//	receiveBaohuParaData2(idAndData, 11);
			//}
			//else if (FunctionNo == 0xA9)//保护阈值帧3
			//{
			//	receiveBaohuParaData3(idAndData, 11);
			//}
			//else if (FunctionNo == 0xAA)//保护阈值帧4
			//{
			//	receiveBaohuParaData4(idAndData, 11);
			//}
			//else if (FunctionNo == 0xAB)//恢复阈值帧1
			//{
			//	receiveHuifuParaData1(idAndData, 11);
			//}
			//else if (FunctionNo == 0xAC)//恢复阈值帧2
			//{
			//	receiveHuifuParaData2(idAndData, 11);
			//}
			//else if (FunctionNo == 0xAD)//恢复阈值帧3
			//{
			//	receiveHuifuParaData3(idAndData, 11);
			//}
			//else if (FunctionNo == 0xAE)//恢复阈值帧4
			//{
			//	receiveHuifuParaData4(idAndData, 11);
			//}
			//else if (FunctionNo == 0xB8)//电压校准参数帧
			//{
			//	receiveVolCal(idAndData, 11);
			//}
			//else if (FunctionNo == 0xB9)//电流校准参数帧
			//{
			//	receiveCurCal(idAndData, 11);
			//}
			//else if (FunctionNo == 0xBA)
			//{
			//	receiveEqualFunState(idAndData, 1);
			//}
			//else if (FunctionNo >= 0xC0 && FunctionNo <= 0xCF)//均衡状态帧
			//{
			//	receiveEqualStateData(idAndData, 11);
			//}

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
	//temp = (quint16)pointer[2];
	//batPackStatData.boardNo = (float)temp / 10;//板卡号，电池组不需要

	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batPackStatData.totalVol = (float)temp / 10;//总电压

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	batPackStatData.totalCur = (float)temp / 10;//总电流

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	batPackStatData.aveVol = temp;//单体平均电压

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	batPackStatData.volDeff = temp;//单体压差

	emit newData(DataType::BatPackStat);
}

//读取电压最值
void DataCenter::receiveVolMaxMin(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	volMaxMinData.maxVol = temp;//max电压,1LSB=1mv

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	volMaxMinData.maxNo = temp;//max电压编号,1LSB=1mv

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	volMaxMinData.minVol = temp;//min电压,1LSB=1mv

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	volMaxMinData.minNo = temp;//min电压编号,1LSB=1mv

	emit newData(DataType::VolMaxMin);
}

void DataCenter::receiveBatTempNum(quint8* pointer, int count)//读取电池和温度总数
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	batTempNumData.batNum = temp;//电池总数

	temp = (quint16)pointer[5];
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
	charDisCutOffData.charCutOffTotalVol = (float)temp / 10;//充电截止总电压，1LSB=0.1V

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	charDisCutOffData.disCutOffTotalVol = (float)temp / 10;//放点截止总电压，1LSB=0.1V

	temp = (quint16)pointer[7];
	temp <<= 8;
	temp |= (quint16)pointer[8];
	charDisCutOffData.maxAllowedCharCur = (float)temp / 10;//最大允许充电电流，1LSB=0.1A

	temp = (quint16)pointer[9];
	temp <<= 8;
	temp |= (quint16)pointer[10];
	charDisCutOffData.maxAllowedDisCur = (float)temp / 10;//最大允许放电电流，1LSB=0.1A

	emit newData(DataType::CharDisCutOff);
}

void DataCenter::receiveSOCSOH(quint8* pointer, int count)
{
	quint16 temp;
	temp = (quint16)pointer[3];
	temp <<= 8;
	temp |= (quint16)pointer[4];
	SOCSOHData.soc = (float)temp / 10;//SOC，1LSB=0.1%

	temp = (quint16)pointer[5];
	temp <<= 8;
	temp |= (quint16)pointer[6];
	SOCSOHData.soh = (float)temp / 10;//放点截止总电压，1LSB=0.1V

	emit newData(DataType::SOCSOH);
}

void DataCenter::receiveBMSState(quint8* pointer, int count)
{
	quint16 temp;
	
	BMSStateData.batState = pointer[3];//电池状态

	temp = (quint16)pointer[4];
	temp <<= 8;
	temp |= (quint16)pointer[5];
	BMSStateData.relayState = temp;//接触器的继电器状态

	temp = (quint16)pointer[6];
	temp <<= 8;
	temp |= (quint16)pointer[7];
	BMSStateData.fetalErr = temp;//一级保护标志

	temp = (quint16)pointer[8];
	temp <<= 8;
	temp |= (quint16)pointer[9];
	BMSStateData.warningErr = temp;//二级报警标志

	emit newData(DataType::BMSState);
}

void DataCenter::receiveCellVol(quint8* pointer, int count)//读取单体电压数据
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

void DataCenter::receiveCellTemp(quint8* pointer, int count)//读取单体电压数据
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

//读取电池组配置信息1
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

//读取电池组配置信息2
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

//读取电池组配置信息3
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

//读取报警参数1
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
//读取报警参数2
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
//读取报警参数3
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
//读取报警参数4
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
//读取保护参数1
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
//读取保护参数2
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
//读取保护参数3
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
//读取保护参数4
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

//读取恢复参数1
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

//读取恢复参数2
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
//读取恢复参数3
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
//读取恢复参数4
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

//读取电压校准参数
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

//读取电流校准参数
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

//读取均衡功能状态信息
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