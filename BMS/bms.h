#pragma once
#pragma pack(1)

#include <QtGlobal>
//定义工程中所用到的结构体和枚举体
namespace BMS {
	//枚举体
	//数据的功能（命令）编码,用于接收或发送数据时表明数据的功能
	enum DataFunc : unsigned char {
		//接收数据类型
		CellVolFunc_Head = 0x00, //单体电压帧头
		CellVolFunc_Tail = 0x63,//单体电压帧尾
		CellTempFunc_Head = 0x64,//单体温度帧头
		CellTempFunc_Tail = 0x95,//单体温度帧尾
		VolMaxMinFunc = 0x96,//电压最值帧
		TempMaxMinFunc = 0x97,//温度最值帧
		BatPackStatFunc = 0x98,//电池组状态信息
		CharDisCutOffFunc = 0x99,//充放电截止信息帧
		SOCSOHFunc = 0x9A,//SOC、SOH信息帧
		BMSStateFunc = 0x9B,//BMS状态信息
		BatPackConf1Func = 0xA0,//电池组配置信息帧1
		BatPackConf2Func = 0xA1,//电池组配置信息帧2
		BatPackConf3Func = 0xA2,//电池组配置信息帧3
		BaojingPara1Func = 0xA3,//报警阈值帧1
		BaojingPara2Func = 0xA4,//报警阈值帧2
		BaojingPara3Func = 0xA5,//报警阈值帧3
		BaojingPara4Func = 0xA6,//报警阈值帧4
		BaohuPara1Func = 0xA7,//保护阈值帧1
		BaohuPara2Func = 0xA8,//保护阈值帧2
		BaohuPara3Func = 0xA9,//保护阈值帧3
		BaohuPara4Func = 0xAA,//保护阈值帧4
		HuifuPara1Func = 0xAB,//恢复阈值帧1
		HuifuPara2Func = 0xAC,//恢复阈值帧2
		HuifuPara3Func = 0xAD,//恢复阈值帧3
		HuifuPara4Func = 0xAE,//恢复阈值帧4
		VolCalFunc = 0xB8,//电压校准参数帧
		CurCalFunc = 0xB9,//电流校准参数帧
		EqualFunStateFunc = 0xBA,//均衡功能状态帧
		EqualStateFunc_Head = 0xC0,//均衡状态帧头
		EqualStateFunc_Tail = 0xCF,//均衡状态帧头

		//发送数据类型

		SaveBatPackConf1 = 0xA0,//电池组配置信息帧1
		SaveBatPackConf2 = 0xA1,//电池组配置信息帧1
		SaveBatPackConf3 = 0xA2,//电池组配置信息帧1
		SaveBaojingPara1 = 0xA3,//报警阈值帧1
		SaveBaojingPara2 = 0xA4,//报警阈值帧1
		SaveBaojingPara3 = 0xA5,//报警阈值帧1
		SaveBaojingPara4 = 0xA6,//报警阈值帧1
		SaveBaohuPara1 = 0xA7,//保护阈值帧1
		SaveBaohuPara2 = 0xA8,//保护阈值帧1
		SaveBaohuPara3 = 0xA9,//保护阈值帧1
		SaveBaohuPara4 = 0xAA,//保护阈值帧1
		SaveHuifuPara1 = 0xAB,//恢复阈值帧1
		SaveHuifuPara2 = 0xAC,//恢复阈值帧2
		SaveHuifuPara3 = 0xAD,//恢复阈值帧3
		SaveHuifuPara4 = 0xAE,//恢复阈值帧4
		SendCommand = 0xB1,//上位机指令帧
		SaveVolCal = 0xB8,//电压校准参数帧
		SaveCurCal = 0xB9,//电流校准参数帧
		SendEqualFunCtrl = 0xBA,//均衡功能开启控制帧
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

	//电池和温度总数数据定义
	struct BatTempNumDataSt {
		int batNum;
		int tempNum;
	};

	//单体电压数据定义
	struct CellVolDataSt {
		int frameNo;//帧标号0~99
		int vol[4];//4个电压数据
	};

	//单体温度数据定义
	struct CellTempDataSt {
		int frameNo;//帧编号100~149
		int temp[8];//8个温度数据

	};

	//均衡状态帧
	struct EqualStateDataSt {
		int frameNo;//帧号
		bool equalType[4];//均衡类型
		quint16 equalNo[4];//电池序号
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
		int maxNo;//Data2：max温度编号
		int minNo;//Data3：min温度编号
		int tempDeff;//Data4：温差
		int aveTemp;//Data5：电池组平均温度
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
		quint16 relayState;//接触器的继电器状态，1-闭合；0-断开
		quint16 fetalErr;//一级保护标志
		quint16 warningErr;//二级报警标志
	};

	//电池组配置信息
	struct BatPackConfData1St {
		int batType;//电芯类型
		float batDesignCapacity;//电池设计容量，1LSB= 0.1Ah
		int bmuNum; //电池组BMU数量
		int cellBatNum;//BMU单体电池数量
	};
	struct BatPackConfData2St {
		int tempSensorNum;//BMU温度传感器数量
		int endSlaveBatNum;//末端从控电池节数
		int endSlaveTempNum;//末端从控温度个数
		float batMaxCapacity;//电池最大容量，1LSB=0.1Ah 
	};
	struct BatPackConfData3St {
		float batCurCapacity;//电池当前容量，1LSB=0.1Ah
		float batCurLeftCapacity;//电池当前剩余容量，1LSB=0.1Ah
		int tempBMUNum;//温度所需BMU数量，1LSB=1
		float equalStartPressure; //均衡开启压差，1LSB=0.1%
	};

	//报警参数
	struct BaojingParaData1St {
		int cellUV;//Data0~1：单体欠压，1LSB=1mV
		int cellOV;//Data2~3：单体过压，1LSB=1mV
		float packUV;//Data4~5：总压欠压，1LSB=0.1V
		float packOV;//Data6~7：总压过压，1LSB=0.1V
	};
	struct BaojingParaData2St {
		int charUT;//Data0~1：充电温度过低，1LSB=1℃，有40度正向偏置
		int charOT;//Data2~3：充电温度过高，1LSB=1℃，有40度正向偏置
		int dischUT;//Data4~5：放电温度过低，1LSB=1℃，有40度正向偏置
		int dischOT;//Data6~7：放电温度过高，1LSB=1℃，有40度正向偏置
	};
	struct BaojingParaData3St {
		float charOC;//Data0~1：充电过流，1LSB=0.1A
		float dischOC;//Data2~3：放电过流，1LSB=0.1A
		int commOT;//Data4~5：从控通信超时时间，1LSB=1s
		int insLow;//Data6~7：绝缘阻值过低，1LSB=1 Ω/V
	};
	struct BaojingParaData4St {
		int volDiff;//Data0~1：压差过大，1LSB=1mV 
		int tempDiff;//Data2~3：温差过大，1LSB=1mV
	};

	//保护参数
	struct BaohuParaData1St {
		int cellUV;//Data0~1：单体欠压，1LSB=1mV
		int cellOV;//Data2~3：单体过压，1LSB=1mV
		float packUV;//Data4~5：总压欠压，1LSB=0.1V
		float packOV;//Data6~7：总压过压，1LSB=0.1V
	};
	struct BaohuParaData2St {
		int charUT;//Data0~1：充电温度过低，1LSB=1℃，有40度正向偏置
		int charOT;//Data2~3：充电温度过高，1LSB=1℃，有40度正向偏置
		int dischUT;//Data4~5：放电温度过低，1LSB=1℃，有40度正向偏置
		int dischOT;//Data6~7：放电温度过高，1LSB=1℃，有40度正向偏置
	};
	struct BaohuParaData3St {
		float charOC;//Data0~1：充电过流，1LSB=0.1A
		float dischOC;//Data2~3：放电过流，1LSB=0.1A
		int commOT;//Data4~5：从控通信超时时间，1LSB=1s
		int insLow;//Data6~7：绝缘阻值过低，1LSB=1 Ω/V
	};
	struct BaohuParaData4St {
		int volDiff;//Data0~1：压差过大，1LSB=1mV 
		int tempDiff;//Data2~3：温差过大，1LSB=1mV
	};

	//恢复参数
	struct HuifuParaData1St {
		int cellUV;//Data0~1：单体欠压，1LSB=1mV
		int cellOV;//Data2~3：单体过压，1LSB=1mV
		float packUV;//Data4~5：总压欠压，1LSB=0.1V
		float packOV;//Data6~7：总压过压，1LSB=0.1V
	};
	struct HuifuParaData2St {
		int charUT;//Data0~1：充电温度过低，1LSB=1℃，有40度正向偏置
		int charOT;//Data2~3：充电温度过高，1LSB=1℃，有40度正向偏置
		int dischUT;//Data4~5：放电温度过低，1LSB=1℃，有40度正向偏置
		int dischOT;//Data6~7：放电温度过高，1LSB=1℃，有40度正向偏置
	};
	struct HuifuParaData3St {
		float charOC;//Data0~1：充电过流，1LSB=0.1A
		float dischOC;//Data2~3：放电过流，1LSB=0.1A
		int commOT;//Data4~5：从控通信超时时间，1LSB=1s
		int insLow;//Data6~7：绝缘阻值过低，1LSB=1 Ω/V
	};
	struct HuifuParaData4St {
		int volDiff;//Data0~1：压差过大，1LSB=1mV 
		int tempDiff;//Data2~3：温差过大，1LSB=1mV
	};

	//电压校准参数
	struct VolCalDataSt {
		qint16 offSet;
		float gain;
	};

	//电流校准参数
	struct CurCalDataSt {
		qint16 offSet; //Data0~1：电压零点偏移，1LSB = 1mV
		float gain;//Data2~3：电压增益，1LSB = 0.001
	};

	//均衡功能状态信息
	struct EqualFunStateDataSt {
		int state;
	};
}
#pragma pack()