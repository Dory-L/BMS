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
		nRec = udpSocket->read(temp, length);
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
			memcpy(idAndData, receiveBuffer + 3, 10);
			recFlag |= OVER_FLAG;
			/*--------*/  switch (receiveBuffer[1])/*--------在此进行数据分发------------*/
			{
			case (quint8)BMS::CommSuccFunc:
				receiveCommSucc(idAndData, 10);
				break;
			case (quint8)BMS::VolCalFunc:
				receiveVolCal(idAndData, 10);
				break;
			case (quint8)BMS::CurCalFunc://GPS信息
				receiveCurCal(idAndData, 10);
				break;
			case (quint8)BMS::BatPackConfFunc://DHT22信息
				receiveBatPackConf(idAndData, 10);
				break;
			case (quint8)BMS::AlaProParFunc://AHRS信息
				receiveAlaProPar(idAndData, 10);
				break;
			defalut://未定义功能号
				emit warning("tcpSocket接收到未定义数据格式！.");
				break;
			}

			recFlag &= ~OVER_FLAG;
		}
	}

	if (bufferIndex == BUFFER_SIZE)//防止溢出
		bufferIndex--;
}

//接收通信成功指令数据
void DataCenter::receiveCommSucc(quint8* pointer, int count)
{
}
//接收电压校准数据
void DataCenter::receiveVolCal(quint8* pointer, int count)
{
}
//接收电流校准数据
void DataCenter::receiveCurCal(quint8* pointer, int count)
{
}
//接收电池组配置数据
void DataCenter::receiveBatPackConf(quint8* pointer, int count)
{
}
//接收报警&保护参数数据
void DataCenter::receiveAlaProPar(quint8* pointer, int count)
{
}

void DataCenter::udpStateChanged(QAbstractSocket::SocketState socketState)
{
}

void DataCenter::udpErrorOccur(QAbstractSocket::SocketError socketError)
{
}