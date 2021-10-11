#pragma once
#pragma pack(1)

#include <QtGlobal>
//定义工程中所用到的结构体和枚举体
namespace BMS {
	//枚举体
	//数据的功能（命令）编码,用于接收或发送数据时表明数据的功能
	enum DataFunc :unsigned char {
		//接收数据类型
		CommSuccFunc = 0x9B,//通信成功指令
		VolCalFunc = 0x9C,//电压校准
		CurCalFunc = 0x9D,//电流校准
		BatPackConfFunc = 0x9E,//电池组配置
		AlaProParFunc = 0x9F,//报警&保护参数

		//发送数据类型
	};

	//通信成功指令数据定义
	struct CommSuccDataSt {
		quint8 boardNo;//板号
		quint8 comSucc;//0x66：读取成功，0x80：通信失败
	};

	//电压校准数据定义
	struct VolCalDataSt {
		quint8 boardNo;//板号
	};

	//电流校准数据定义
	struct CurCalDataSt {
		quint8 boardNo;//板号
	};

	//电池组配置数据定义
	struct BatPackConfDataSt {
		quint8 boardNo;//板号
	};

	//报警&保护参数数据定义
	struct AlaProParDataSt {
		quint8 boardNo;//板号
	};
}
#pragma pack()