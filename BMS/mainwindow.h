#pragma once

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QMessageBox>
#include <QTimer>
#include "ui_mainwindow.h"
#include "dialog.h"
#include "DataCenter.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
signals:

private:
    Ui::MainWindowClass ui;
	Dialog* dlg;//udp�Ի���
	QString aimIp;//Ŀ��ip
	int aimPort;//Ŀ��˿�
	DataCenter* dataCenter;//��������

	void iniConnect();//��ʼ���źŲ�
	void guiInitate();//��ʼ������

	QTimer *timer;

	//�����غ��¶�����
	int m_batTotalNum;
	int m_tempTotalNum;

	//���չ�������ʱ�洢�����ѹ���¶�
	QVector<int> m_volVec;
	QVector<int> m_tempVec;

	//�������
	QVector<int> m_volVecComplete;
	QVector<int> m_tempVecComplete;

	//����
	QVector<bool> equalStateVec;
	QVector<int>  equalNoVec;

	//���ݽ��ռ���
	int m_volCount;
	int m_tempCount;

private slots:
	void on_actionopenudp_triggered();//��udp�¼�
	void on_actioncloseudp_triggered();//�ر�udp�¼�
	void on_dataFlushBtn_clicked();//����ˢ���¼�
	void on_dataCheckBox_stateChanged(int state);//�Զ�ˢ���¼�
	void on_volCalBtn_clicked();//��ѹУ׼��������

	//��DataCenterͨ��
	void dataChange(DataCenter::DataType type);
	void dataCenterError(QString str);
	void messageFromDataCenter(QString str);
};
