#pragma once
#pragma pack(1)

#include <QtGlobal>
//���幤�������õ��Ľṹ���ö����
namespace BMS {
	//ö����
	//���ݵĹ��ܣ��������,���ڽ��ջ�������ʱ�������ݵĹ���
	enum DataFunc : unsigned char {
		//������������
		BatPackStatFunc = 0x98,//�����״̬��Ϣ

		//������������
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
		quint8 relayState;//�Ӵ����ļ̵���״̬��1-�պϣ�0-�Ͽ�
		quint16 fetalErr;//һ��������־
		quint16 warningErr;//����������־
		float totalVolThreshold;//�ܵ�ѹ��ֵ1LSB=0.1V��������ܵ�ѹ>����ֵ������һ���̵����Ͽ�ָ�
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
		int equalStartPressure;//���⿪��ѹ�1LSB = 1mV
		int equalClosePressure; //����ر�ѹ�1LSB = 1mV
	};
}
#pragma pack()