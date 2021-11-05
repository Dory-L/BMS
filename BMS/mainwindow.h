#pragma once

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QMessageBox>
#include <QTimer>
#include "ui_mainwindow.h"
#include "dialog.h"
#include "DataCenter.h"
#include "qcustomplot.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

	void setupRealtimeData(QCustomPlot *customPlotV, QCustomPlot *customPlotA);//��ʼ����̬����
signals:
private:
    Ui::MainWindowClass ui;
	Dialog* dlg;//udp�Ի���
	DataCenter* dataCenter;//��������

	void iniConnect();//��ʼ���źŲ�
	void guiInitate();//��ʼ������
	QByteArray QString2Hex(QString str);//�ַ���ת����16������
	char ConvertHexChar(char ch);

	QTimer *timer;
	QTimer dataTimer;
	QLabel *aimLabel;
	QLabel *localLabel;

	//�����غ��¶�����
	int m_batTotalNum;
	int m_tempTotalNum;

	//���չ�������ʱ�洢�����ѹ���¶�
	QVector<int> m_volVec;//��ѹֵ
	QVector<int> m_volNoVec;//��ѹ���
	QVector<int> m_tempVec;//�¶�ֵ
	QVector<int> m_tempNoVec;//�¶ȱ��
	QVector<bool> m_equalStateVec;//����״̬
	QVector<int>  m_equalNoVec;//���������

	QMap<int, int> m_volMap;
	QMap<int, int> m_tempMap;
	QMap<int, bool> m_equalMap;

	//�������
	QVector<int> m_volVecComplete;
	QVector<int> m_tempVecComplete;

	//�����ѹ���������
	float m_totalVol;
	float m_totalCur;

	//fps����
	int m_receFps;
	int m_sendFPs;

private slots:
	void realtimeDataSlot();////���ʵʱ���ݲ�

	void on_actionopenudp_triggered();//��udp�¼�
	void on_actioncloseudp_triggered();//�ر�udp�¼�
	void on_actionstart_triggered();//ϵͳ�����¼�

	void dataFlushBtn_clicked();//����ˢ���¼�
	void dataCheckBox_stateChanged(int state);//�Զ�ˢ���¼�
	void volCalBtn_clicked();//��ѹУ׼��������
	void curCalBtn_clicked();//����У׼��������

	void readBatPackBtn_clicked();//��ȡ��������ò�����Ϣ
	void saveBatPackBtn_clicked();//�����������ò�����Ϣ
	void readYuZhiBtn_clicked();//��ȡ��ֵ����
	void saveYuZhiBtn_clicked();//������ֵ����
	void confirmBtn_clicked();//�̵��������¼�
	void readVolCal_clicked();//��ȡ��ѹУ׼����
	void saveVolCal_clicked();//�����ѹУ׼����
	void readCurCalBtn_clicked();//��ȡ����У׼����
	void saveCurCalBtn_clicked();//�������У׼����
	void equalStartBtn_clicked(); //���⹦��״̬����
	void equalCloseBtn_clicked();//���⹦��״̬�ر�

	void on_updateBtn_clicked();//ˢ�µ�һҳ
	void on_pauseBtn_clicked();//��ͣ��ʾ�¼�
	void on_clearBtn_clicked();//�����ʾ�¼�
	void on_manualSendBtn_clicked();//�ֶ������¼�

	//��DataCenterͨ��
	void dataChange(DataCenter::DataType type);
	void dataCenterError(QString str);
	void messageFromDataCenter(QString str);
	void displayReceiveData(quint8 data[], int n);
	void displaySendData(char data[], int n);

	//��dialogͨ��
	void aimOK_Slot(int aimPort, QString aimIp);
	void localOk_Slot(int localPort);
};
