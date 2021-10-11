#pragma once
#pragma pack(1)

#include <QtGlobal>
//���幤�������õ��Ľṹ���ö����
namespace BMS {
	//ö����
	//���ݵĹ��ܣ��������,���ڽ��ջ�������ʱ�������ݵĹ���
	enum DataFunc :unsigned char {
		//������������
		CommSuccFunc = 0x9B,//ͨ�ųɹ�ָ��
		VolCalFunc = 0x9C,//��ѹУ׼
		CurCalFunc = 0x9D,//����У׼
		BatPackConfFunc = 0x9E,//���������
		AlaProParFunc = 0x9F,//����&��������

		//������������
	};

	//ͨ�ųɹ�ָ�����ݶ���
	struct CommSuccDataSt {
		quint8 boardNo;//���
		quint8 comSucc;//0x66����ȡ�ɹ���0x80��ͨ��ʧ��
	};

	//��ѹУ׼���ݶ���
	struct VolCalDataSt {
		quint8 boardNo;//���
	};

	//����У׼���ݶ���
	struct CurCalDataSt {
		quint8 boardNo;//���
	};

	//������������ݶ���
	struct BatPackConfDataSt {
		quint8 boardNo;//���
	};

	//����&�����������ݶ���
	struct AlaProParDataSt {
		quint8 boardNo;//���
	};
}
#pragma pack()