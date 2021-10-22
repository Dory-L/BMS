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
	DataCenter* dataCenter;//数据中心

	void iniConnect();//初始化信号槽
	void guiInitate();//初始化界面

	QTimer *timer;

	//单体电池和温度数量
	int m_batTotalNum;
	int m_tempTotalNum;

	//接收过程在暂时存储单体电压和温度
	QVector<int> m_volVec;//电压值
	QVector<int> m_volNoVec;//电压编号
	QVector<int> m_tempVec;//温度值
	QVector<int> m_tempNoVec;//温度编号
	QVector<bool> m_equalStateVec;//均衡状态
	QVector<int>  m_equalNoVec;//均衡电池序号

	QMap<int, int> m_volMap;
	QMap<int, int> m_tempMap;
	QMap<int, bool> m_equalMap;

	//接收完成
	QVector<int> m_volVecComplete;
	QVector<int> m_tempVecComplete;

	//数据接收计数
	int m_volCount;
	int m_tempCount;

private slots:
	void on_actionopenudp_triggered();//打开udp事件
	void on_actioncloseudp_triggered();//关闭udp事件
	void on_actionstart_triggered();//系统启动事件

	void dataFlushBtn_clicked();//数据刷新事件
	void dataCheckBox_stateChanged(int state);//自动刷新事件
	void volCalBtn_clicked();//电压校准参数计算
	void curCalBtn_clicked();//电流校准参数计算

	void readBatPackBtn_clicked();//读取电池组配置参数信息
	void saveBatPackBtn_clicked();//保存电池组配置参数信息
	void readYuZhiBtn_clicked();//读取阈值参数
	void saveYuZhiBtn_clicked();//保存阈值参数
	void confirmBtn_clicked();//继电器控制事件
	void readVolCal_clicked();//读取电压校准参数
	void saveVolCal_clicked();//保存电压校准参数
	void readCurCalBtn_clicked();//读取电流校准参数
	void saveCurCalBtn_clicked();//保存电流校准参数
	void equalStartBtn_clicked(); //均衡功能状态开启
	void equalCloseBtn_clicked();//均衡功能状态关闭

	//与DataCenter通信
	void dataChange(DataCenter::DataType type);
	void dataCenterError(QString str);
	void messageFromDataCenter(QString str);
};
