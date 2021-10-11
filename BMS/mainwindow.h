#pragma once

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QUdpSocket>
#include <QMessageBox>
#include "ui_mainwindow.h"
#include "dialog.h"
#include "DataCenter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

	QUdpSocket *udpSocket;

private:
    Ui::MainWindowClass ui;
	Dialog* dlg;//udp�Ի���
	QString aimIp;//Ŀ��ip
	int aimPort;//Ŀ��˿�
	DataCenter *dataCenter;//��������

	void iniConnect();//��ʼ���źŲ�
	void guiInitate();//��ʼ������

private slots:
	void on_actionopenudp_triggered();//��udp
	void on_actioncloseudp_triggered();//�ر�udp

	//��DataCenterͨ��
	void dataChange(DataCenter::DataType type);
	void dataCenterError(QString str);
	void messageFromDataCenter(QString str);
};
