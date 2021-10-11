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
	Dialog* dlg;//udp对话框
	QString aimIp;//目标ip
	int aimPort;//目标端口
	DataCenter *dataCenter;//数据中心

	void iniConnect();//初始化信号槽
	void guiInitate();//初始化界面

private slots:
	void on_actionopenudp_triggered();//打开udp
	void on_actioncloseudp_triggered();//关闭udp

	//与DataCenter通信
	void dataChange(DataCenter::DataType type);
	void dataCenterError(QString str);
	void messageFromDataCenter(QString str);
};
