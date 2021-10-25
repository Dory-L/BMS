#pragma once
#pragma pack(1)

#include <QtGlobal>
//���幤�������õ��Ľṹ���ö����
namespace BMS {
	//ö����
	//���ݵĹ��ܣ��������,���ڽ��ջ�������ʱ�������ݵĹ���
	enum DataFunc : unsigned char {
		//������������
		CellVolFunc_Head = 0x00, //�����ѹ֡ͷ
		CellVolFunc_Tail = 0x63,//�����ѹ֡β
		CellTempFunc_Head = 0x64,//�����¶�֡ͷ
		CellTempFunc_Tail = 0x95,//�����¶�֡β
		VolMaxMinFunc = 0x96,//��ѹ��ֵ֡
		TempMaxMinFunc = 0x97,//�¶���ֵ֡
		BatPackStatFunc = 0x98,//�����״̬��Ϣ
		CharDisCutOffFunc = 0x99,//��ŵ��ֹ��Ϣ֡
		SOCSOHFunc = 0x9A,//SOC��SOH��Ϣ֡
		BMSStateFunc = 0x9B,//BMS״̬��Ϣ
		BatPackConf1Func = 0xA0,//�����������Ϣ֡1
		BatPackConf2Func = 0xA1,//�����������Ϣ֡2
		BatPackConf3Func = 0xA2,//�����������Ϣ֡3
		BaojingPara1Func = 0xA3,//������ֵ֡1
		BaojingPara2Func = 0xA4,//������ֵ֡2
		BaojingPara3Func = 0xA5,//������ֵ֡3
		BaojingPara4Func = 0xA6,//������ֵ֡4
		BaohuPara1Func = 0xA7,//������ֵ֡1
		BaohuPara2Func = 0xA8,//������ֵ֡2
		BaohuPara3Func = 0xA9,//������ֵ֡3
		BaohuPara4Func = 0xAA,//������ֵ֡4
		HuifuPara1Func = 0xAB,//�ָ���ֵ֡1
		HuifuPara2Func = 0xAC,//�ָ���ֵ֡2
		HuifuPara3Func = 0xAD,//�ָ���ֵ֡3
		HuifuPara4Func = 0xAE,//�ָ���ֵ֡4
		VolCalFunc = 0xB8,//��ѹУ׼����֡
		CurCalFunc = 0xB9,//����У׼����֡
		EqualFunStateFunc = 0xBA,//���⹦��״̬֡
		EqualStateFunc_Head = 0xC0,//����״̬֡ͷ
		EqualStateFunc_Tail = 0xCF,//����״̬֡ͷ

		//������������

		SaveBatPackConf1 = 0xA0,//�����������Ϣ֡1
		SaveBatPackConf2 = 0xA1,//�����������Ϣ֡1
		SaveBatPackConf3 = 0xA2,//�����������Ϣ֡1
		SaveBaojingPara1 = 0xA3,//������ֵ֡1
		SaveBaojingPara2 = 0xA4,//������ֵ֡1
		SaveBaojingPara3 = 0xA5,//������ֵ֡1
		SaveBaojingPara4 = 0xA6,//������ֵ֡1
		SaveBaohuPara1 = 0xA7,//������ֵ֡1
		SaveBaohuPara2 = 0xA8,//������ֵ֡1
		SaveBaohuPara3 = 0xA9,//������ֵ֡1
		SaveBaohuPara4 = 0xAA,//������ֵ֡1
		SaveHuifuPara1 = 0xAB,//�ָ���ֵ֡1
		SaveHuifuPara2 = 0xAC,//�ָ���ֵ֡2
		SaveHuifuPara3 = 0xAD,//�ָ���ֵ֡3
		SaveHuifuPara4 = 0xAE,//�ָ���ֵ֡4
		SendCommand = 0xB1,//��λ��ָ��֡
		SaveVolCal = 0xB8,//��ѹУ׼����֡
		SaveCurCal = 0xB9,//����У׼����֡
		SendEqualFunCtrl = 0xBA,//���⹦�ܿ�������֡
	};

	enum BMSState : quint16 {
		//������־
		ERR_CELL_UV = 0x0001,//�����ѹǷѹ
		ERR_CELL_OV = 0x0002,//�����ѹ��ѹ
		ERR_PACK_UV = 0x0004,//�ܵ�ѹǷѹ
		ERR_PACK_OV = 0x0008,//�ܵ�ѹ��ѹ
		ERR_CHR_UT = 0x0010,//����¶ȹ���
		ERR_CHR_OT = 0x0020,////����¶ȹ���
		ERR_DISCH_UT = 0x0040,//�ŵ��¶ȹ���
		ERR_DISCH_OT = 0x0080,//�ŵ��¶ȹ���
		ERR_CHR_OC = 0x0100,//������
		ERR_DISCH_OC = 0x0200,//�ŵ����
		ERR_BMU_COMM = 0x0400,//BMUͨ���쳣
		ERR_INS_LOW = 0x0800,//��Ե����
		ERR_VOLT_DIFF = 0x1000,//ѹ�����
		ERR_TEMP_DIFF = 0x2000,//�²����
	};

	//��غ��¶��������ݶ���
	struct BatTempNumDataSt {
		int batNum;
		int tempNum;
	};

	//�����ѹ���ݶ���
	struct CellVolDataSt {
		int frameNo;//֡���0~99
		int vol[4];//4����ѹ����
	};

	//�����¶����ݶ���
	struct CellTempDataSt {
		int frameNo;//֡���100~149
		int temp[8];//8���¶�����

	};

	//����״̬֡
	struct EqualStateDataSt {
		int frameNo;//֡��
		bool equalType[4];//��������
		quint16 equalNo[4];//������
	};

	//�����״̬��Ϣ���ݶ���
	struct BatPackStatDataSt {
		float totalVol;//Data0~1��������ܵ�ѹ��1LSB=0.1V
		float totalCur;//Data2~3��������ܵ�����1LSB=0.1A
		int aveVol;//Data4~5������ƽ����ѹ��1LSB=1mV
		int volDeff;//Data6~7������ѹ�1LSB=1mV
	};

	//��ѹ��ֵ���ݶ���1LSB=1mv
	struct VolMaxMinDataSt {
		int maxVol;//Data0~1��max��ѹ 
		int maxNo;//Data2~3��max��ѹ���
		int minVol;//Data4~5��min��ѹ
		int minNo;//Data6~7��min��ѹ���
	};

	//�¶���ֵ���ݶ���
	struct TempMaxMinDataSt {
		int tempMax;//Data0��max�¶�,1LSB=1�棬��40������ƫ�ã������յ�����Ϊ41����ʵ���¶�Ϊ1�ȡ�
		int tempMin;//Data1��min�¶�
		int maxNo;//Data2��max�¶ȱ��
		int minNo;//Data3��min�¶ȱ��
		int tempDeff;//Data4���²�
		int aveTemp;//Data5�������ƽ���¶�
	};

	//��ŵ��ֹ��Ϣ���ݶ���
	struct CharDisCutOffDataSt {
		float charCutOffTotalVol;//Data0~1������ֹ�ܵ�ѹ��1LSB=0.1V
		float disCutOffTotalVol;//Data2~3���ŵ��ֹ�ܵ�ѹ��1LSB=0.1V
		float maxAllowedCharCur;//Data4~5����������������1LSB=0.1A
		float maxAllowedDisCur;//Data6~7���������ŵ������1LSB=0.1A
	};

	//SOC��SOH��Ϣ���ݶ���
	struct SOCSOHDataSt {
		float soc;//Data0~1��SOC��1LSB=0.1%
		float soh;//Data2~3��SOH��1LSB=0.1%
	};

	//BMS״̬���ݶ���
	struct BMSStateDataSt {
		quint8 batState;//���״̬������IDLE_STATE-0�����CHARGE_STATE-1���ŵ�DISCHARGE_STATE-2
		quint16 relayState;//�Ӵ����ļ̵���״̬��1-�պϣ�0-�Ͽ�
		quint16 fetalErr;//һ��������־
		quint16 warningErr;//����������־
	};

	//�����������Ϣ
	struct BatPackConfData1St {
		int batType;//��о����
		float batDesignCapacity;//������������1LSB= 0.1Ah
		int bmuNum; //�����BMU����
		int cellBatNum;//BMU����������
	};
	struct BatPackConfData2St {
		int tempSensorNum;//BMU�¶ȴ���������
		int endSlaveBatNum;//ĩ�˴ӿص�ؽ���
		int endSlaveTempNum;//ĩ�˴ӿ��¶ȸ���
		float batMaxCapacity;//������������1LSB=0.1Ah 
	};
	struct BatPackConfData3St {
		float batCurCapacity;//��ص�ǰ������1LSB=0.1Ah
		float batCurLeftCapacity;//��ص�ǰʣ��������1LSB=0.1Ah
		int tempBMUNum;//�¶�����BMU������1LSB=1
		float equalStartPressure; //���⿪��ѹ�1LSB=0.1%
	};

	//��������
	struct BaojingParaData1St {
		int cellUV;//Data0~1������Ƿѹ��1LSB=1mV
		int cellOV;//Data2~3�������ѹ��1LSB=1mV
		float packUV;//Data4~5����ѹǷѹ��1LSB=0.1V
		float packOV;//Data6~7����ѹ��ѹ��1LSB=0.1V
	};
	struct BaojingParaData2St {
		int charUT;//Data0~1������¶ȹ��ͣ�1LSB=1�棬��40������ƫ��
		int charOT;//Data2~3������¶ȹ��ߣ�1LSB=1�棬��40������ƫ��
		int dischUT;//Data4~5���ŵ��¶ȹ��ͣ�1LSB=1�棬��40������ƫ��
		int dischOT;//Data6~7���ŵ��¶ȹ��ߣ�1LSB=1�棬��40������ƫ��
	};
	struct BaojingParaData3St {
		float charOC;//Data0~1����������1LSB=0.1A
		float dischOC;//Data2~3���ŵ������1LSB=0.1A
		int commOT;//Data4~5���ӿ�ͨ�ų�ʱʱ�䣬1LSB=1s
		int insLow;//Data6~7����Ե��ֵ���ͣ�1LSB=1 ��/V
	};
	struct BaojingParaData4St {
		int volDiff;//Data0~1��ѹ�����1LSB=1mV 
		int tempDiff;//Data2~3���²����1LSB=1mV
	};

	//��������
	struct BaohuParaData1St {
		int cellUV;//Data0~1������Ƿѹ��1LSB=1mV
		int cellOV;//Data2~3�������ѹ��1LSB=1mV
		float packUV;//Data4~5����ѹǷѹ��1LSB=0.1V
		float packOV;//Data6~7����ѹ��ѹ��1LSB=0.1V
	};
	struct BaohuParaData2St {
		int charUT;//Data0~1������¶ȹ��ͣ�1LSB=1�棬��40������ƫ��
		int charOT;//Data2~3������¶ȹ��ߣ�1LSB=1�棬��40������ƫ��
		int dischUT;//Data4~5���ŵ��¶ȹ��ͣ�1LSB=1�棬��40������ƫ��
		int dischOT;//Data6~7���ŵ��¶ȹ��ߣ�1LSB=1�棬��40������ƫ��
	};
	struct BaohuParaData3St {
		float charOC;//Data0~1����������1LSB=0.1A
		float dischOC;//Data2~3���ŵ������1LSB=0.1A
		int commOT;//Data4~5���ӿ�ͨ�ų�ʱʱ�䣬1LSB=1s
		int insLow;//Data6~7����Ե��ֵ���ͣ�1LSB=1 ��/V
	};
	struct BaohuParaData4St {
		int volDiff;//Data0~1��ѹ�����1LSB=1mV 
		int tempDiff;//Data2~3���²����1LSB=1mV
	};

	//�ָ�����
	struct HuifuParaData1St {
		int cellUV;//Data0~1������Ƿѹ��1LSB=1mV
		int cellOV;//Data2~3�������ѹ��1LSB=1mV
		float packUV;//Data4~5����ѹǷѹ��1LSB=0.1V
		float packOV;//Data6~7����ѹ��ѹ��1LSB=0.1V
	};
	struct HuifuParaData2St {
		int charUT;//Data0~1������¶ȹ��ͣ�1LSB=1�棬��40������ƫ��
		int charOT;//Data2~3������¶ȹ��ߣ�1LSB=1�棬��40������ƫ��
		int dischUT;//Data4~5���ŵ��¶ȹ��ͣ�1LSB=1�棬��40������ƫ��
		int dischOT;//Data6~7���ŵ��¶ȹ��ߣ�1LSB=1�棬��40������ƫ��
	};
	struct HuifuParaData3St {
		float charOC;//Data0~1����������1LSB=0.1A
		float dischOC;//Data2~3���ŵ������1LSB=0.1A
		int commOT;//Data4~5���ӿ�ͨ�ų�ʱʱ�䣬1LSB=1s
		int insLow;//Data6~7����Ե��ֵ���ͣ�1LSB=1 ��/V
	};
	struct HuifuParaData4St {
		int volDiff;//Data0~1��ѹ�����1LSB=1mV 
		int tempDiff;//Data2~3���²����1LSB=1mV
	};

	//��ѹУ׼����
	struct VolCalDataSt {
		qint16 offSet;
		float gain;
	};

	//����У׼����
	struct CurCalDataSt {
		qint16 offSet; //Data0~1����ѹ���ƫ�ƣ�1LSB = 1mV
		float gain;//Data2~3����ѹ���棬1LSB = 0.001
	};

	//���⹦��״̬��Ϣ
	struct EqualFunStateDataSt {
		int state;
	};
}
#pragma pack()