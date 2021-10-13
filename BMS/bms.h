#pragma once
#pragma pack(1)

#include <QtGlobal>
//定义工程中所用到的结构体和枚举体
namespace BMS {
	//枚举体
	//数据的功能（命令）编码,用于接收或发送数据时表明数据的功能
	enum DataFunc : unsigned char {
		//接收数据类型
		BatPackStatFunc = 0x98,//电池组状态信息

		//发送数据类型
	};

	enum BMSState : quint16 {
		//保护标志
		ERR_CELL_UV = 0x0001,//单体电压欠压
		ERR_CELL_OV = 0x0002,//单体电压过压
		ERR_PACK_UV = 0x0004,//总电压欠压
		ERR_PACK_OV = 0x0008,//总电压过压
		ERR_CHR_UT = 0x0010,//充电温度过低
		ERR_CHR_OT = 0x0020,////充电温度过高
		ERR_DISCH_UT = 0x0040,//放电温度过低
		ERR_DISCH_OT = 0x0080,//放电温度过高
		ERR_CHR_OC = 0x0100,//充电过流
		ERR_DISCH_OC = 0x0200,//放电过流
		ERR_BMU_COMM = 0x0400,//BMU通信异常
		ERR_INS_LOW = 0x0800,//绝缘过低
		ERR_VOLT_DIFF = 0x1000,//压差过大
		ERR_TEMP_DIFF = 0x2000,//温差过大
	};
	//单体电压数据定义
	struct CellVolDataSt {
		int frameNo;//帧标号0~99
		int vol1;
		int vol2;
		int vol3;
		int vol4;
	};

	struct CellTempDataSt {
		int frameNo;//帧编号100~149
		int temp1;
		int temp2;
		int temp3;
		int temp4;
		int temp5;
		int temp6;
		int temp7;
		int temp8;
	};

	//电池组状态信息数据定义
	struct BatPackStatDataSt {
		float totalVol;//Data0~1：电池组总电压，1LSB=0.1V
		float totalCur;//Data2~3：电池组总电流，1LSB=0.1A
		int aveVol;//Data4~5：单体平均电压，1LSB=1mV
		int volDeff;//Data6~7：单体压差，1LSB=1mV
	};

	//电压最值数据定义1LSB=1mv
	struct VolMaxMinDataSt {
		int maxVol;//Data0~1：max电压 
		int maxNo;//Data2~3：max电压编号
		int minVol;//Data4~5：min电压
		int minNo;//Data6~7：min电压编号
	};

	//温度最值数据定义
	struct TempMaxMinDataSt {
		int tempMax;//Data0：max温度,1LSB=1℃，有40度正向偏置，例如收到数据为41，则实际温度为1度。
		int tempMin;//Data1：min温度
		int maxNo;//Data2~3：max温度编号
		int minNo;//Data4~5：min温度编号
		int tempDeff;//Data6：温差
		int aveTemp;//Data7：电池组平均温度
	};

	//充放电截止信息数据定义
	struct CharDisCutOffDataSt {
		float charCutOffTotalVol;//Data0~1：充电截止总电压，1LSB=0.1V
		float disCutOffTotalVol;//Data2~3：放点截止总电压，1LSB=0.1V
		float maxAllowedCharCur;//Data4~5：最大允许充电电流，1LSB=0.1A
		float maxAllowedDisCur;//Data6~7：最大允许放电电流，1LSB=0.1A
	};

	//SOC、SOH信息数据定义
	struct SOCSOHDataSt {
		float soc;//Data0~1：SOC，1LSB=0.1%
		float soh;//Data2~3：SOH，1LSB=0.1%
	};

	//BMS状态数据定义
	struct BMSStateDataSt {
		quint8 batState;//电池状态，静置IDLE_STATE-0；充电CHARGE_STATE-1；放电DISCHARGE_STATE-2
		quint8 relayState;//接触器的继电器状态，1-闭合；0-断开
		quint16 fetalErr;//一级保护标志
		quint16 warningErr;//二级报警标志
		float totalVolThreshold;//总电压阈值1LSB=0.1V，电池组总电压>该阈值，发送一个继电器断开指令。
	};
}
#pragma pack()