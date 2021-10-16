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
	Dialog* dlg;//udp对话框
	QString aimIp;//目标ip
	int aimPort;//目标端口
	DataCenter* dataCenter;//数据中心

	void iniConnect();//初始化信号槽
	void guiInitate();//初始化界面

	QTimer *timer;

	//单体电池和温度数量
	int m_batTotalNum;
	int m_tempTotalNum;

	//接收过程在暂时存储单体电压和温度
	QVector<int> m_volVec;
	QVector<int> m_tempVec;

	//接收完成
	QVector<int> m_volVecComplete;
	QVector<int> m_tempVecComplete;

	//均衡
	QVector<bool> equalStateVec;
	QVector<int>  equalNoVec;

	//数据接收计数
	int m_volCount;
	int m_tempCount;

private slots:
	void on_actionopenudp_triggered();//打开udp事件
	void on_actioncloseudp_triggered();//关闭udp事件
	void on_dataFlushBtn_clicked();//数据刷新事件
	void on_dataCheckBox_stateChanged(int state);//自动刷新事件
	void on_volCalBtn_clicked();//电压校准参数计算

	//与DataCenter通信
	void dataChange(DataCenter::DataType type);
	void dataCenterError(QString str);
	void messageFromDataCenter(QString str);
};
