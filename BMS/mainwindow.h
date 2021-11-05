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

	void setupRealtimeData(QCustomPlot *customPlotV, QCustomPlot *customPlotA);//初始化动态曲线
signals:
private:
    Ui::MainWindowClass ui;
	Dialog* dlg;//udp对话框
	DataCenter* dataCenter;//数据中心

	void iniConnect();//初始化信号槽
	void guiInitate();//初始化界面
	QByteArray QString2Hex(QString str);//字符串转换成16进制数
	char ConvertHexChar(char ch);

	QTimer *timer;
	QTimer dataTimer;
	QLabel *aimLabel;
	QLabel *localLabel;

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

	//总体电压和总体电流
	float m_totalVol;
	float m_totalCur;

	//fps计数
	int m_receFps;
	int m_sendFPs;

private slots:
	void realtimeDataSlot();////添加实时数据槽

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

	void on_updateBtn_clicked();//刷新第一页
	void on_pauseBtn_clicked();//暂停显示事件
	void on_clearBtn_clicked();//清空显示事件
	void on_manualSendBtn_clicked();//手动发送事件

	//与DataCenter通信
	void dataChange(DataCenter::DataType type);
	void dataCenterError(QString str);
	void messageFromDataCenter(QString str);
	void displayReceiveData(quint8 data[], int n);
	void displaySendData(char data[], int n);

	//与dialog通信
	void aimOK_Slot(int aimPort, QString aimIp);
	void localOk_Slot(int localPort);
};
