#include "mainwindow.h"
#include <windows.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	
	dataCenter = new DataCenter(this);
	//this->setWindowIcon(QIcon(":/images/BMS.ico"));
	//this->setIconSize(QSize(500, 500));
	QIcon tubiao(":/images/favicon.ico");
	this->setWindowIcon(tubiao);
	timer = new QTimer(this);
	iniConnect();
	guiInitate();

	m_receFps = 0;
	m_sendFPs = 0;
}

void MainWindow::iniConnect()
{
	connect(ui.actionopenudp, SIGNAL(triggered), this, SLOT(on_actionopenudp_triggered));
	connect(ui.actioncloseudp, SIGNAL(triggered), this, SLOT(on_actioncloseudp_triggered));
	connect(timer, SIGNAL(timeout()), this, SLOT(dataFlushBtn_clicked()));
	connect(dataCenter, &DataCenter::receiveOriginalData, this, &MainWindow::displayReceiveData);
	connect(dataCenter, &DataCenter::sendOriginalData, this, &MainWindow::displaySendData);
	//dataCenter
	connect(dataCenter, &DataCenter::newData, this, &MainWindow::dataChange);
}

void MainWindow::guiInitate()
{ 
	ui.groupBox_8->layout()->setMargin(3);
	//ui.tab_6->layout()->setMargin(2);
	//动态曲线
	setupRealtimeData(ui.customPlotV, ui.customPlotA);
	/*单体电压表格*/
	QStringList header;
	header << u8"序号" << u8"电压(mV)" << u8"均衡状态";
	ui.tableWidget->setHorizontalHeaderLabels(header);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);//填充表头
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//均分填充
	ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");//表头样式
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑

	/*单体温度表格*/
	header.clear();
	header << u8"序号" << u8"温度(℃)";
	ui.tableWidget_2->setHorizontalHeaderLabels(header);
	ui.tableWidget_2->horizontalHeader()->setStretchLastSection(true);//填充表头
	ui.tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//均分填充
	ui.tableWidget_2->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");//表头样式
	ui.tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑

	/*均衡状态表格*/
	header.clear();
	header << u8"序号" << u8"均衡状态";
	ui.tableWidget_3->setHorizontalHeaderLabels(header);
	ui.tableWidget_3->horizontalHeader()->setStretchLastSection(true);//填充表头
	ui.tableWidget_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//均分填充
	ui.tableWidget_3->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");//表头样式
	ui.tableWidget_3->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑

	//刷新设置
	QIntValidator* IntValidator = new QIntValidator(this);
	IntValidator->setRange(1, 60);//可以改成（-255,255），这样就是负数
	ui.periodLineEdit->setValidator(IntValidator);
	ui.periodLineEdit->setPlaceholderText(u8"请输入1-60");//背景提示用户输入范围
							   
	ui.lcdNumber->setDigitCount(4);// 设置能显示的位数
	ui.lcdNumber->setMode(QLCDNumber::Dec);// 设置显示的模式为十进制
	ui.lcdNumber->setDigitCount(4);// 设置能显示的位数
	ui.lcdNumber->setMode(QLCDNumber::Dec);// 设置显示的模式为十进制

	//状态栏
	ui.statusBar->setSizeGripEnabled(false);//去掉状态栏右下角的三角
	aimLabel = new QLabel(this);
	localLabel = new QLabel(this);
	localLabel->setText(u8"本地端口:");
	aimLabel->setText(u8"目标IP:		目标端口:	");
	QFont ft;
	ft.setPointSize(8);
	aimLabel->setFont(ft);
	localLabel->setFont(ft);
	ui.statusBar->addWidget(localLabel, 1);
	ui.statusBar->addWidget(aimLabel, 1);
}

//初始化动态曲线
void MainWindow::setupRealtimeData(QCustomPlot *customPlotV, QCustomPlot *customPlotA)
{
	//customPlot->setBackground(QBrush(Qt::transparent)); 
	customPlotV->addGraph(); // blue line
	customPlotV->graph(0)->setPen(QPen(QColor(40, 110, 255)));
	customPlotV->xAxis->setBasePen(QPen(Qt::blue, 4));
	customPlotV->yAxis->setBasePen(QPen(Qt::blue, 4));
	customPlotV->xAxis->setLabel(u8"时间(单位：秒)");
	customPlotV->yAxis->setLabel(u8"总电压(单位：V)");

	customPlotA->addGraph(); // blue line
	customPlotA->graph(0)->setPen(QPen(QColor(40, 110, 255)));
	customPlotA->xAxis->setBasePen(QPen(Qt::blue, 4));
	customPlotA->yAxis->setBasePen(QPen(Qt::blue, 4));
	customPlotA->xAxis->setLabel(u8"时间(单位：秒)");
	customPlotA->yAxis->setLabel(u8"总电流(单位：A)");

	QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
	timeTicker->setTimeFormat("%h:%m:%s");
	customPlotV->xAxis->setTicker(timeTicker);
	customPlotV->axisRect()->setupFullAxesBox();
	customPlotV->yAxis->setRange(0, 1000);

	customPlotA->xAxis->setTicker(timeTicker);
	customPlotA->axisRect()->setupFullAxesBox();
	customPlotA->yAxis->setRange(-100, 100);

	// make left and bottom axes transfer their ranges to right and top axes:
	connect(customPlotV->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlotV->xAxis2, SLOT(setRange(QCPRange)));
	connect(customPlotV->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlotV->yAxis2, SLOT(setRange(QCPRange)));
	connect(customPlotA->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlotA->xAxis2, SLOT(setRange(QCPRange)));
	connect(customPlotA->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlotA->yAxis2, SLOT(setRange(QCPRange)));

	// setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
	connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
	
}

void MainWindow::realtimeDataSlot()
{
	static QTime time(QTime::currentTime());
	// calculate two new data points:
	double key = time.elapsed() / 1000.0; // time elapsed since start of demo, in seconds
	static double lastPointKey = 0;
	if (key - lastPointKey > 1) // at most add point every 1 s
	{
		// add data to lines:
		ui.customPlotV->graph(0)->addData(key, m_totalVol);
		ui.customPlotA->graph(0)->addData(key, m_totalCur);
		// rescale value (vertical) axis to fit the current data:
		ui.customPlotV->graph(0)->rescaleValueAxis(true, true);
		ui.customPlotA->graph(0)->rescaleValueAxis(true, true);
		lastPointKey = key;
	}
	// make key axis range scroll with the data (at a constant range size of 8):
	ui.customPlotV->xAxis->setRange(key, 8, Qt::AlignRight);
	ui.customPlotV->replot();
	ui.customPlotA->xAxis->setRange(key, 8, Qt::AlignRight);
	ui.customPlotA->replot();

	// calculate frames per second:
	//static double lastFpsKey;
	//static int frameCount;
	//++frameCount;
	//if (key - lastFpsKey > 2) // average fps over 2 seconds
	//{
	//	ui.statusBar->showMessage(
	//		QString("%1 FPS, Total Data points: %2")
	//		.arg(frameCount / (key - lastFpsKey), 0, 'f', 0)
	//		.arg(ui.customPlotV->graph(0)->data()->size() + ui.customPlotV->graph(1)->data()->size())
	//		, 0);
	//	lastFpsKey = key;
	//	frameCount = 0;
	//}
}

void MainWindow::displayReceiveData(quint8 data[], int n)
{
	QString temp;
	temp = "88 10 ";
	for (int i = 0; i < n; i++)
	{
		temp += QString("%1").arg((quint8)data[i],2, 16, QLatin1Char('0')).toUpper();
		//temp += QString::number((quint8)data[i], 16);
		temp += " ";
	}

	ui.textBrowser->append(QString(u8"[RX %1]：%2").arg(QTime::currentTime().toString("HH:mm:ss")).arg(temp));
	m_receFps++;
	ui.lcdNumber->display(QString::number(m_receFps));
}

void MainWindow::displaySendData(char data[], int n)
{
	QString temp;
	for (int i = 0; i < n; i++)
	{
		temp += QString("%1").arg((quint8)data[i], 2, 16, QLatin1Char('0')).toUpper();
		//temp += QString::number((quint8)data[i], 16);
		temp += " ";
	}
	ui.textBrowser->append(QString(u8"[TX %1]：%2").arg(QTime::currentTime().toString("HH:mm:ss")).arg(temp));
	m_sendFPs++;
	ui.lcdNumber_2->display(QString::number(m_sendFPs));
}

void MainWindow::on_actionopenudp_triggered()
{
	dlg = new Dialog();
	connect(dlg, &Dialog::aimOk_Signal, this, &MainWindow::aimOK_Slot);
	connect(dlg, &Dialog::localOk_Signal, this, &MainWindow::localOk_Slot);
	int ret = dlg->exec();

	delete dlg;
	dlg = NULL;
}

void MainWindow::aimOK_Slot(int aimPort, QString aimIp)
{
	dataCenter->m_aimIp = QHostAddress(aimIp);
	dataCenter->m_aimPort = aimPort;
	QMessageBox::information(this, u8"提示", u8"目标IP和端口绑定成功！");

	QString aimStr = QString(u8"目标IP:%1 目标端口:%2").arg(aimIp).arg(QString::number(aimPort));
	aimLabel->setText(aimStr);
}

void MainWindow::localOk_Slot(int localPort)
{
	if ((dataCenter->udpSocket->bind(localPort) == true))
	{
		QMessageBox::information(this, u8"提示", u8"UDP端口绑定成功！");
		
		QString localStr = QString(u8"本地端口:%1").arg(localPort);
		localLabel->setText(localStr);
	}
	else
		QMessageBox::warning(this, u8"警告", u8"UDP打开失败！");
}

void MainWindow::on_actioncloseudp_triggered()
{
	dataCenter->udpSocket->close();
	QMessageBox::information(this, u8"提示", u8"UDP端口关闭成功!");
	localLabel->setText(u8"本地端口:");
}

//系统启动事件
void MainWindow::on_actionstart_triggered()
{
	char buf[8];
	buf[0] = 3;
	for (int i = 1; i < 8; i++)
		buf[i] = 0;

	int ret = dataCenter->sendDataToUdp(buf, 8, BMS::SendCommand);
	if (ret == -1)
	{
		QMessageBox::warning(this, u8"错误", u8"请检查网络连接!");
	}
	else {
		dataTimer.start(0); // Interval 0 means to refresh as fast as possible
	}
}

//数据中心错误
void MainWindow::dataCenterError(QString str)
{
}

//数据中心消息
void MainWindow::messageFromDataCenter(QString str)
{
}

// 更新数据
void MainWindow::dataChange(DataCenter::DataType type)
{
	switch (type)
	{
	case DataCenter::CellVol://单体电压
		BMS::CellVolDataSt cellVol = dataCenter->getCellVolData();
		for (int i = 0; i < 4; i++)
		{
			if (cellVol.vol[i] != 0xFFFF)
				m_volMap[cellVol.frameNo * 4 + i] = cellVol.vol[i];
		}
		break;
	case DataCenter::CellTemp://单体温度
		BMS::CellTempDataSt cellTemp = dataCenter->getCellTempData();
		for (int i = 0; i < 8; i++)
		{
			if (cellTemp.temp[i] != 0xFF - 40)
				m_tempMap[cellTemp.frameNo * 8 + i] = cellTemp.temp[i];
		}
		break;
	case DataCenter::EqualState://均衡状态
		BMS::EqualStateDataSt equalState = dataCenter->getEqualStateData();
		for (int i = 0; i < 4; i++)
		{
			if (equalState.equalNo[i] != 0)
				m_equalMap[equalState.equalNo[i]] = equalState.equalType[i];
		}
		break;
	case DataCenter::BatPackStat://电池组状态
		BMS::BatPackStatDataSt batPack = dataCenter->getBatPackStatData();
		/*在界面显示信息*/
		m_totalVol = batPack.totalVol;
		m_totalCur = batPack.totalCur;
		ui.lineEdit_totalV->setText(QString::number(batPack.totalVol));
		ui.lineEdit_totalA->setText(QString::number(batPack.totalCur));
		ui.lineEdit_aveV->setText(QString::number(batPack.aveVol));
		ui.lineEdit_vDeff->setText(QString::number(batPack.volDeff));
		break;
	case DataCenter::VolMaxMin://电压最值
		BMS::VolMaxMinDataSt volMaxMin = dataCenter->getVolMaxMinData();
		/*界面显示信息*/
		ui.lineEdit_maxV->setText(QString::number(volMaxMin.maxVol));
		ui.lineEdit_maxVNo->setText(QString::number(volMaxMin.maxNo));
		ui.lineEdit_minV->setText(QString::number(volMaxMin.minVol));
		ui.lineEdit_minVNo->setText(QString::number(volMaxMin.minNo));
		break;
	case DataCenter::TempMaxMin://温度最值
		BMS::TempMaxMinDataSt tempMaxMinData = dataCenter->getTempMaxMinData();
		/*界面显示信息*/
		ui.lineEdit_maxT->setText(QString::number(tempMaxMinData.tempMax));
		ui.lineEdit_minT->setText(QString::number(tempMaxMinData.tempMin));
		ui.lineEdit_MaxTNo->setText(QString::number(tempMaxMinData.maxNo));
		ui.lineEdit_minTNo->setText(QString::number(tempMaxMinData.minNo));
		ui.lineEdit_TDeff->setText(QString::number(tempMaxMinData.tempDeff));
		ui.lineEdit_aveT->setText(QString::number(tempMaxMinData.aveTemp));
		break;
	case DataCenter::CharDisCutOff://充放电截止
		BMS::CharDisCutOffDataSt charDisCutOffData = dataCenter->getCharDisCutOffData();
		/*界面显示信息*/
		ui.lineEdit_chargeCutOffTotalV->setText(QString::number(charDisCutOffData.charCutOffTotalVol));
		ui.lineEdit_dischargeCutOffTotalV->setText(QString::number(charDisCutOffData.disCutOffTotalVol));
		ui.lineEdit_maxAllowedChargeA->setText(QString::number(charDisCutOffData.maxAllowedCharCur));
		ui.lineEdit_maxAllowedDischargeA->setText(QString::number(charDisCutOffData.maxAllowedDisCur));
		break;
	case DataCenter::SOCSOH:
		BMS::SOCSOHDataSt SOCSOHData = dataCenter->getSOCSOHData();
		/*界面显示信息*/
		ui.lineEdit_SOC->setText(QString::number(SOCSOHData.soc));
		ui.lineEdit_SOH->setText(QString::number(SOCSOHData.soh));
		break;
	case DataCenter::BMSState://BMS状态
	{
		BMS::BMSStateDataSt	BMSStateData = dataCenter->getBMSStateData();
		/*界面显示信息*/
		switch (BMSStateData.batState)//电池状态
		{
		case 0:
			ui.lineEdit_batStatet->setText(u8"静置");
			break;
		case 1:
			ui.lineEdit_batStatet->setText(u8"充电");
			break;
		case 2:
			ui.lineEdit_batStatet->setText(u8"f放电");
			break;
		default:
			break;
		}

		switch (BMSStateData.relayState)//继电器状态
		{
		case 0:
			ui.lineEdit_relayState->setText(u8"断开");
			break;
		case 1:
			ui.lineEdit_relayState->setText(u8"闭合");
			break;
		default:
			break;
		}

		/*正常*/
		quint16 ERR_FLAG = BMSStateData.fetalErr | BMSStateData.warningErr;
		ERR_FLAG = ~ERR_FLAG;//为1的位为正常
		if (ERR_FLAG & BMS::ERR_CELL_UV)//单体电压欠压
			ui.label_cellQainYa->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_CELL_OV)//单体电压过压
			ui.label_cellGuoYa->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_PACK_UV)//总电压欠压
			ui.label_packQianYa->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_PACK_OV)//总电压过压
			ui.label_packGuoYa->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_CHR_UT)//充电温度过低
			ui.label_chDiWen->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_CHR_OT)//充电温度过高
			ui.label_chGaoWen->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_DISCH_UT)//放电温度过低
			ui.label_disDiWen->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_DISCH_OT)//放电温度过高
			ui.label_disGaoWen->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_CHR_OC)//充电过流
			ui.label_chGuoLiu->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_DISCH_OC)//放电过流
			ui.label_disGuoLiu->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_BMU_COMM)//BMU通信异常
			ui.label_tongXinGuZhang->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_INS_LOW)//绝缘过低
			ui.label_jueYuanGuzhang->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_VOLT_DIFF)//压差过大
			ui.label_yaChaGuoDa->setPixmap(QPixmap(u8":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_TEMP_DIFF)//温差过大
			ui.label_wenChaGuoDa->setPixmap(QPixmap(u8":/images/正常.ico"));

		/*二级保护*/
		if (BMSStateData.warningErr & BMS::ERR_CELL_UV)//单体电压欠压
			ui.label_cellQainYa->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CELL_OV)//单体电压过压
			ui.label_cellGuoYa->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_PACK_UV)//总电压欠压
			ui.label_packQianYa->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_PACK_OV)//总电压过压
			ui.label_packGuoYa->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CHR_UT)//充电温度过低
			ui.label_chDiWen->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CHR_OT)//充电温度过高
			ui.label_chGaoWen->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_DISCH_UT)//放电温度过低
			ui.label_disDiWen->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_DISCH_OT)//放电温度过高
			ui.label_disGaoWen->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CHR_OC)//充电过流
			ui.label_chGuoLiu->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_DISCH_OC)//放电过流
			ui.label_disGuoLiu->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_BMU_COMM)//BMU通信异常
			ui.label_tongXinGuZhang->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_INS_LOW)//绝缘过低
			ui.label_jueYuanGuzhang->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_VOLT_DIFF)//压差过大
			ui.label_yaChaGuoDa->setPixmap(QPixmap(u8":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_TEMP_DIFF)//温差过大
			ui.label_wenChaGuoDa->setPixmap(QPixmap(u8":/images/报警.ico"));

		/*一级保护*/
		if (BMSStateData.fetalErr & BMS::ERR_CELL_UV)//单体电压欠压
			ui.label_cellQainYa->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CELL_OV)//单体电压过压
			ui.label_cellGuoYa->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_PACK_UV)//总电压欠压
			ui.label_packQianYa->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_PACK_OV)//总电压过压
			ui.label_packGuoYa->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CHR_UT)//充电温度过低
			ui.label_chDiWen->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CHR_OT)//充电温度过高
			ui.label_chGaoWen->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_DISCH_UT)//放电温度过低
			ui.label_disDiWen->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_DISCH_OT)//放电温度过高
			ui.label_disGaoWen->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CHR_OC)//充电过流
			ui.label_chGuoLiu->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_DISCH_OC)//放电过流
			ui.label_disGuoLiu->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_BMU_COMM)//BMU通信异常
			ui.label_tongXinGuZhang->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_INS_LOW)//绝缘过低
			ui.label_jueYuanGuzhang->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_VOLT_DIFF)//压差过大
			ui.label_yaChaGuoDa->setPixmap(QPixmap(u8":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_TEMP_DIFF)//温差过大
			ui.label_wenChaGuoDa->setPixmap(QPixmap(u8":/images/保护.ico"));

		break;
	}
	case DataCenter::VolCal:
		BMS::VolCalDataSt volCal = dataCenter->getVolCalData();

		ui.lineEdit_5->setText(QString::number(volCal.offSet));
		ui.lineEdit_6->setText(QString::number(volCal.gain, 'f', 3));
		break;
	case DataCenter::CurCal:
		BMS::CurCalDataSt curCal = dataCenter->getCurCalData();
		ui.lineEdit_11->setText(QString::number((float)curCal.offSet / 10, 'f', 1));
		ui.lineEdit_12->setText(QString::number(curCal.gain, 'f', 3));
		break;
	case DataCenter::BatPackConf1:
		BMS::BatPackConfData1St	batPackConf1 = dataCenter->getBatPackConfData1();
		ui.comboBox->setCurrentIndex(batPackConf1.batType);
		ui.lineEdit_13->setText(QString::number(batPackConf1.batDesignCapacity));
		ui.lineEdit_14->setText(QString::number(batPackConf1.bmuNum));
		ui.lineEdit_15->setText(QString::number(batPackConf1.cellBatNum));
		break;
	case DataCenter::BatPackConf2:
		BMS::BatPackConfData2St	batPackConf2 = dataCenter->getBatPackConfData2();
		ui.lineEdit_16->setText(QString::number(batPackConf2.tempSensorNum));
		ui.lineEdit_17->setText(QString::number(batPackConf2.endSlaveBatNum));
		ui.lineEdit_18->setText(QString::number(batPackConf2.endSlaveTempNum));
		ui.lineEdit_20->setText(QString::number(batPackConf2.batMaxCapacity));
		break;
	case DataCenter::BatPackConf3:
		BMS::BatPackConfData3St	batPackConf3 = dataCenter->getBatPackConfData3();

		ui.lineEdit_21->setText(QString::number(batPackConf3.batCurCapacity));
		ui.lineEdit_22->setText(QString::number(batPackConf3.batCurLeftCapacity));
		ui.lineEdit_19->setText(QString::number(batPackConf3.tempBMUNum));
		ui.lineEdit_23->setText(QString::number(batPackConf3.equalStartPressure));
		break;
	case DataCenter::BaojingPara1:
		BMS::BaojingParaData1St baojingPara1 = dataCenter->getBaojingParaData1();
		ui.lineEdit_24->setText(QString::number(baojingPara1.cellUV));
		ui.lineEdit_25->setText(QString::number(baojingPara1.cellOV));
		ui.lineEdit_26->setText(QString::number(baojingPara1.packUV, 'f', 1));
		ui.lineEdit_27->setText(QString::number(baojingPara1.packOV, 'f', 1));
		break;
	case DataCenter::BaojingPara2:
		BMS::BaojingParaData2St baojingPara2 = dataCenter->getBaojingParaData2();
		ui.lineEdit_28->setText(QString::number(baojingPara2.charUT));
		ui.lineEdit_29->setText(QString::number(baojingPara2.charOT));
		ui.lineEdit_30->setText(QString::number(baojingPara2.dischUT));
		ui.lineEdit_31->setText(QString::number(baojingPara2.dischOT));
		break;
	case DataCenter::BaojingPara3:
		BMS::BaojingParaData3St baojingPara3 = dataCenter->getBaojingParaData3();
		ui.lineEdit_32->setText(QString::number(baojingPara3.charOC));
		ui.lineEdit_33->setText(QString::number(baojingPara3.dischOC));
		ui.lineEdit_34->setText(QString::number(baojingPara3.commOT));
		ui.lineEdit_35->setText(QString::number(baojingPara3.insLow));
		break;
	case DataCenter::BaojingPara4:
		BMS::BaojingParaData4St baojingPara4 = dataCenter->getBaojingParaData4();
		ui.lineEdit_36->setText(QString::number(baojingPara4.volDiff));
		ui.lineEdit_37->setText(QString::number(baojingPara4.tempDiff));
		break;
	case DataCenter::BaohuPara1:
		BMS::BaohuParaData1St baohuPara1 = dataCenter->getBaohuParaData1();
		ui.lineEdit_38->setText(QString::number(baohuPara1.cellUV));
		ui.lineEdit_39->setText(QString::number(baohuPara1.cellOV));
		ui.lineEdit_40->setText(QString::number(baohuPara1.packUV, 'f', 1));
		ui.lineEdit_41->setText(QString::number(baohuPara1.packOV, 'f', 1));
		break;
	case DataCenter::BaohuPara2:
		BMS::BaohuParaData2St baohuPara2 = dataCenter->getBaohuParaData2();
		ui.lineEdit_42->setText(QString::number(baohuPara2.charUT));
		ui.lineEdit_43->setText(QString::number(baohuPara2.charOT));
		ui.lineEdit_44->setText(QString::number(baohuPara2.dischUT));
		ui.lineEdit_45->setText(QString::number(baohuPara2.dischOT));
		break;
	case DataCenter::BaohuPara3:
		BMS::BaohuParaData3St baohuPara3 = dataCenter->getBaohuParaData3();
		ui.lineEdit_46->setText(QString::number(baohuPara3.charOC));
		ui.lineEdit_47->setText(QString::number(baohuPara3.dischOC));
		ui.lineEdit_48->setText(QString::number(baohuPara3.commOT));
		ui.lineEdit_49->setText(QString::number(baohuPara3.insLow));
		break;
	case DataCenter::BaohuPara4:
		BMS::BaohuParaData4St baohuPara4 = dataCenter->getBaohuParaData4();
		ui.lineEdit_50->setText(QString::number(baohuPara4.volDiff));
		ui.lineEdit_51->setText(QString::number(baohuPara4.tempDiff));
		break;
	case DataCenter::HuifuPara1:
		BMS::HuifuParaData1St huifuPara1 = dataCenter->getHuifuParaData1();
		ui.lineEdit_52->setText(QString::number(huifuPara1.cellUV));
		ui.lineEdit_53->setText(QString::number(huifuPara1.cellOV));
		ui.lineEdit_54->setText(QString::number(huifuPara1.packUV, 'f', 1));
		ui.lineEdit_55->setText(QString::number(huifuPara1.packOV, 'f', 1));
		break;
	case DataCenter::HuifuPara2:
		BMS::HuifuParaData2St huifuPara2 = dataCenter->getHuifuParaData2();
		ui.lineEdit_56->setText(QString::number(huifuPara2.charUT));
		ui.lineEdit_57->setText(QString::number(huifuPara2.charOT));
		ui.lineEdit_58->setText(QString::number(huifuPara2.dischUT));
		ui.lineEdit_59->setText(QString::number(huifuPara2.dischOT));
		break;
	case DataCenter::HuifuPara3:
		BMS::HuifuParaData3St huifuPara3 = dataCenter->getHuifuParaData3();
		ui.lineEdit_60->setText(QString::number(huifuPara3.charOC));
		ui.lineEdit_61->setText(QString::number(huifuPara3.dischOC));
		ui.lineEdit_62->setText(QString::number(huifuPara3.commOT));
		ui.lineEdit_63->setText(QString::number(huifuPara3.insLow));
		break;
	case DataCenter::HuifuPara4:
		BMS::HuifuParaData4St huifuPara4 = dataCenter->getHuifuParaData4();
		ui.lineEdit_64->setText(QString::number(huifuPara4.volDiff));
		ui.lineEdit_65->setText(QString::number(huifuPara4.tempDiff));
		break;
	case DataCenter::EqualFunState:
		BMS::EqualFunStateDataSt equalFunState = dataCenter->getEqualFunStateData();
		QString state;
		if (equalFunState.state)
			state = u8"开启";
		else 
			state = u8"关闭";
		ui.lineEdit_66->setText(state);
		break;
	}
}

//刷新表格
void MainWindow::dataFlushBtn_clicked()
{
	ui.tableWidget->setRowCount(m_volMap.size());
	QMap<int, int>::const_iterator iterator_1 = m_volMap.constBegin();
	while (iterator_1 != m_volMap.constEnd()) {
		ui.tableWidget->setItem(iterator_1.key(), 0, new QTableWidgetItem(QString::number(iterator_1.key() + 1)));
		ui.tableWidget->setItem(iterator_1.key(), 1, new QTableWidgetItem(QString::number(iterator_1.value())));
		++iterator_1;
	}

	//单体温度
	ui.tableWidget_2->setRowCount(m_tempMap.size());
	QMap<int, int>::const_iterator iterator_2 = m_tempMap.constBegin();
	while (iterator_2 != m_tempMap.constEnd()) {
		ui.tableWidget_2->setItem(iterator_2.key(), 0, new QTableWidgetItem(QString::number(iterator_2.key() + 1)));
		ui.tableWidget_2->setItem(iterator_2.key(), 1, new QTableWidgetItem(QString::number(iterator_2.value())));
		++iterator_2;
	}

	//均衡
	QTableWidgetItem *item = new QTableWidgetItem();
	ui.tableWidget_3->setRowCount(m_equalMap.size());
	int i = 0;
	QString item1;
	for (QMap<int, bool>::const_iterator iterator_3 = m_equalMap.constBegin(); iterator_3 != m_equalMap.constEnd(); iterator_3++)
	{
		if (iterator_3.value())
		{
			item->setText(u8"主动均衡");
			item1 = u8"主动均衡";
		}
		else
		{
			item->setText(u8"被动均衡");
			item1 = u8"被动均衡";
		}
		ui.tableWidget_3->setItem(i, 0, new QTableWidgetItem(QString::number(iterator_3.key())));
		ui.tableWidget_3->setItem(i, 1, new QTableWidgetItem(item1));

		i++;
	}

	m_volMap.clear();
	m_tempMap.clear();
	m_equalMap.clear();
}

//自动刷新事件
void MainWindow::dataCheckBox_stateChanged(int state)
{
	if (state == Qt::Checked) // "选中"
	{
		int interval = ui.periodLineEdit->text().toInt();
		if (interval > 0)
			timer->start(1000 * interval);
		ui.periodLineEdit->setReadOnly(true);
	} 
	else // 未选中 - Qt::Unchecked
	{
		timer->stop();
		ui.periodLineEdit->setReadOnly(false);
	}
}

//电压校准参数计算事件
void MainWindow::volCalBtn_clicked()
{
	float ceH = ui.lineEdit_1->text().toFloat();
	float ceL = ui.lineEdit_2->text().toFloat();
	float zhenH = ui.lineEdit_3->text().toFloat();
	float zhenL = ui.lineEdit_4->text().toFloat();

	if (ceH == ceL)
	{
		QMessageBox::critical(this, u8"错误提示", "value error!");
		return;
	}

	float gain = (zhenH - zhenL) / (ceH - ceL);
	float offset = ceL * gain - zhenL;

	ui.lineEdit_5->setText(QString::number(offset));
	ui.lineEdit_6->setText(QString::number(gain, 'f', 3));
}

//电流校准参数计算事件
void MainWindow::curCalBtn_clicked()
{
	float ceH = ui.lineEdit_7->text().toFloat();
	float ceL = ui.lineEdit_8->text().toFloat();
	float zhenH = ui.lineEdit_9->text().toFloat();
	float zhenL = ui.lineEdit_10->text().toFloat();

	if (ceH == ceL)
	{
		QMessageBox::critical(this, u8"错误提示", "value error!");
		return;
	}

	float gain = (zhenH - zhenL) / (ceH - ceL);
	float offset = ceL * gain - zhenL;

	ui.lineEdit_11->setText(QString::number(offset));
	ui.lineEdit_12->setText(QString::number(gain, 'f', 3));
}

//读取电池组配置参数信息
void MainWindow::readBatPackBtn_clicked()
{
	//清空显示
	ui.comboBox->setCurrentIndex(0);
	ui.lineEdit_13->setText("");
	ui.lineEdit_14->setText("");
	ui.lineEdit_15->setText("");
	ui.lineEdit_16->setText("");
	ui.lineEdit_17->setText("");
	ui.lineEdit_18->setText("");
	ui.lineEdit_19->setText("");
	ui.lineEdit_20->setText("");
	ui.lineEdit_21->setText("");
	ui.lineEdit_22->setText("");
	ui.lineEdit_23->setText("");

	quint8 ps = 0xB1;
	char buf[8];
	
	buf[0] = 1;
	buf[1] = 0;
	for (int i = 2; i < 8; i++)
		buf[i] = 0;

	int ret = dataCenter->sendDataToUdp(buf, 8, BMS::SendCommand);
	if (ret == -1)
		QMessageBox::warning(this, u8"错误", u8"读取失败！");
}

//读取阈值参数
void MainWindow::readYuZhiBtn_clicked()
{
	//清空显示
	ui.lineEdit_24->setText("");
	ui.lineEdit_25->setText("");
	ui.lineEdit_26->setText("");
	ui.lineEdit_27->setText("");
	ui.lineEdit_28->setText("");
	ui.lineEdit_29->setText("");
	ui.lineEdit_30->setText("");
	ui.lineEdit_31->setText("");
	ui.lineEdit_32->setText("");
	ui.lineEdit_33->setText("");
	ui.lineEdit_34->setText("");
	ui.lineEdit_35->setText("");
	ui.lineEdit_36->setText("");
	ui.lineEdit_37->setText("");
	ui.lineEdit_38->setText("");
	ui.lineEdit_39->setText("");
	ui.lineEdit_40->setText("");
	ui.lineEdit_41->setText("");
	ui.lineEdit_42->setText("");
	ui.lineEdit_43->setText("");
	ui.lineEdit_44->setText("");
	ui.lineEdit_45->setText("");
	ui.lineEdit_46->setText("");
	ui.lineEdit_47->setText("");
	ui.lineEdit_48->setText("");
	ui.lineEdit_49->setText("");
	ui.lineEdit_50->setText("");
	ui.lineEdit_51->setText("");
	ui.lineEdit_52->setText("");
	ui.lineEdit_53->setText("");
	ui.lineEdit_54->setText("");
	ui.lineEdit_55->setText("");
	ui.lineEdit_56->setText("");
	ui.lineEdit_57->setText("");
	ui.lineEdit_58->setText("");
	ui.lineEdit_59->setText("");
	ui.lineEdit_60->setText("");
	ui.lineEdit_61->setText("");
	ui.lineEdit_62->setText("");
	ui.lineEdit_63->setText("");
	ui.lineEdit_64->setText("");
	ui.lineEdit_65->setText("");

	char buf[8];
	buf[0] = 1;
	buf[1] = 1;
	for (int i = 2; i < 8; i++)
		buf[i] = 0;

	int ret = dataCenter->sendDataToUdp(buf, 8, BMS::SendCommand);
	if (ret == -1)
		QMessageBox::warning(this, u8"错误", u8"读取失败！");
}

//保存电池组配置参数信息
void MainWindow::saveBatPackBtn_clicked()
{
	quint8 ps1 = 0xA0;//帧1
	quint8 ps2 = 0xA1;//帧2
	quint8 ps3 = 0xA2;//帧3

	quint16 temp;

	char buf1[8];
	char buf2[8];
	char buf3[8];
	char buf4[8];

	//帧1
	buf1[0] = 0;
	buf1[1] = ui.comboBox->currentIndex();
	temp = 10 * ui.lineEdit_13->text().toFloat();
	buf1[2] = temp >> 8;
	buf1[3] = temp & 0xff;
	temp = ui.lineEdit_14->text().toInt();
	buf1[4] = temp >> 8;
	buf1[5] = temp & 0xff;
	temp = ui.lineEdit_15->text().toInt();
	buf1[6] = temp >> 8;
	buf1[7] = temp & 0xff;
	
	//帧2
	temp = ui.lineEdit_16->text().toInt();
	buf2[0] = temp >> 8;
	buf2[1] = temp & 0xff;
	temp = ui.lineEdit_17->text().toFloat();
	buf2[2] = temp >> 8;
	buf2[3] = temp & 0xff;
	temp = ui.lineEdit_18->text().toInt();
	buf2[4] = temp >> 8;
	buf2[5] = temp & 0xff;
	temp = 10 * ui.lineEdit_20->text().toFloat();
	buf2[6] = temp >> 8;
	buf2[7] = temp & 0xff;

	//帧3
	temp = 10 * ui.lineEdit_21->text().toFloat();
	buf3[0] = temp >> 8;
	buf3[1] = temp & 0xff;
	temp = 10 * ui.lineEdit_22->text().toFloat();
	buf3[2] = temp >> 8;
	buf3[3] = temp & 0xff;
	temp = ui.lineEdit_19->text().toInt();
	buf3[4] = temp >> 8;
	buf3[5] = temp & 0xff;
	temp = 10 * ui.lineEdit_23->text().toFloat();
	buf3[6] = temp >> 8;
	buf3[7] = temp & 0xff;

	int ret1 = dataCenter->sendDataToUdp(buf1, 8, BMS::SaveBatPackConf1);
	int ret2 = dataCenter->sendDataToUdp(buf2, 8, BMS::SaveBatPackConf2);
	int ret3 = dataCenter->sendDataToUdp(buf3, 8, BMS::SaveBatPackConf3);
	if (ret1 == -1 || ret2 == -1 || ret3 == -1)
	{
		QMessageBox::warning(this, u8"错误", u8"保存失败，请检查网络连接！");
	}
}

//保存阈值参数
void MainWindow::saveYuZhiBtn_clicked()
{
	quint16 temp;
	int ret1;
	int ret2;
	int ret3;
	int ret4;

	//报警阈值参数
	char bjBuf1[8];
	char bjBuf2[8];
	char bjBuf3[8];
	char bjBuf4[8];

	temp = ui.lineEdit_24->text().toInt();
	bjBuf1[0] = temp >> 8;
	bjBuf1[1] = temp & 0xff;
	temp = ui.lineEdit_25->text().toInt();
	bjBuf1[2] = temp >> 8;
	bjBuf1[3] = temp & 0xff;
	temp = 10 * ui.lineEdit_26->text().toFloat();
	bjBuf1[4] = temp >> 8;
	bjBuf1[5] = temp & 0xff;
	temp = 10 * ui.lineEdit_27->text().toFloat();
	bjBuf1[6] = temp >> 8;
	bjBuf1[7] = temp & 0xff;
	ret1 = dataCenter->sendDataToUdp(bjBuf1, 8, BMS::SaveBaojingPara1);

	temp = ui.lineEdit_28->text().toInt() + 40;
	bjBuf2[0] = temp >> 8;
	bjBuf2[1] = temp & 0xff;
	temp = ui.lineEdit_29->text().toInt() + 40;
	bjBuf2[2] = temp >> 8;
	bjBuf2[3] = temp & 0xff;
	temp = ui.lineEdit_30->text().toInt() + 40;
	bjBuf2[4] = temp >> 8;
	bjBuf2[5] = temp & 0xff;
	temp = ui.lineEdit_31->text().toInt() + 40;
	bjBuf2[6] = temp >> 8;
	bjBuf2[7] = temp & 0xff;
	ret2 = dataCenter->sendDataToUdp(bjBuf2, 8, BMS::SaveBaojingPara2);

	temp = 10 * ui.lineEdit_32->text().toFloat();
	bjBuf3[0] = temp >> 8;
	bjBuf3[1] = temp & 0xff;
	temp = 10 * ui.lineEdit_33->text().toFloat();
	bjBuf3[2] = temp >> 8;
	bjBuf3[3] = temp & 0xff;
	temp = ui.lineEdit_34->text().toInt();
	bjBuf3[4] = temp >> 8;
	bjBuf3[5] = temp & 0xff;
	temp = ui.lineEdit_35->text().toInt();
	bjBuf3[6] = temp >> 8;
	bjBuf3[7] = temp & 0xff;
	ret3 = dataCenter->sendDataToUdp(bjBuf3, 8, BMS::SaveBaojingPara3);

	temp = ui.lineEdit_36->text().toInt();
	bjBuf4[0] = temp >> 8;
	bjBuf4[1] = temp & 0xff;
	temp = ui.lineEdit_37->text().toInt();
	bjBuf4[2] = temp >> 8;
	bjBuf4[3] = temp & 0xff;
	bjBuf4[4] = 0;
	bjBuf4[5] = 0;
	bjBuf4[6] = 0;
	bjBuf4[7] = 0;

	ret4 = dataCenter->sendDataToUdp(bjBuf4, 8, BMS::SaveBaojingPara4);
	if (ret1 == -1 || ret2 == -1 || ret3 == -1 || ret4 == -1)
	{
		QMessageBox::warning(this, u8"错误", u8"保存失败，请检查网络连接！");
		return;
	}

	Sleep(20);

	//保护阈值参数
	char bhBuf1[8];
	char bhBuf2[8];
	char bhBuf3[8];
	char bhBuf4[8];

	temp = ui.lineEdit_38->text().toInt();
	bhBuf1[0] = temp >> 8;
	bhBuf1[1] = temp & 0xff;
	temp = ui.lineEdit_39->text().toInt();
	bhBuf1[2] = temp >> 8;
	bhBuf1[3] = temp & 0xff;
	temp = 10 * ui.lineEdit_40->text().toFloat();
	bhBuf1[4] = temp >> 8;
	bhBuf1[5] = temp & 0xff;
	temp = 10 * ui.lineEdit_41->text().toFloat();
	bhBuf1[6] = temp >> 8;
	bhBuf1[7] = temp & 0xff;
	ret1 = dataCenter->sendDataToUdp(bhBuf1, 8, BMS::SaveBaohuPara1);

	temp = ui.lineEdit_42->text().toInt() + 40;
	bhBuf2[0] = temp >> 8;
	bhBuf2[1] = temp & 0xff;
	temp = ui.lineEdit_43->text().toInt() + 40;
	bhBuf2[2] = temp >> 8;
	bhBuf2[3] = temp & 0xff;
	temp = ui.lineEdit_44->text().toInt() + 40;
	bhBuf2[4] = temp >> 8;
	bhBuf2[5] = temp & 0xff;
	temp = ui.lineEdit_45->text().toInt() + 40;
	bhBuf2[6] = temp >> 8;
	bhBuf2[7] = temp & 0xff;
	ret2 = dataCenter->sendDataToUdp(bhBuf2, 8, BMS::SaveBaohuPara2);

	temp = 10 * ui.lineEdit_46->text().toFloat();
	bhBuf3[0] = temp >> 8;
	bhBuf3[1] = temp & 0xff;
	temp = 10 * ui.lineEdit_47->text().toFloat();
	bhBuf3[2] = temp >> 8;
	bhBuf3[3] = temp & 0xff;
	temp = ui.lineEdit_48->text().toInt();
	bhBuf3[4] = temp >> 8;
	bhBuf3[5] = temp & 0xff;
	temp = ui.lineEdit_49->text().toInt();
	bhBuf3[6] = temp >> 8;
	bhBuf3[7] = temp & 0xff;
	ret3 = dataCenter->sendDataToUdp(bhBuf3, 8, BMS::SaveBaohuPara3);

	temp = ui.lineEdit_50->text().toInt();
	bhBuf4[0] = temp >> 8;
	bhBuf4[1] = temp & 0xff;
	temp = ui.lineEdit_51->text().toInt();
	bhBuf4[2] = temp >> 8;
	bhBuf4[3] = temp & 0xff;
	bhBuf4[4] = 0;
	bhBuf4[5] = 0;
	bhBuf4[6] = 0;
	bhBuf4[7] = 0;
	ret4 = dataCenter->sendDataToUdp(bhBuf4, 8, BMS::SaveBaohuPara4);

	if (ret1 == -1 || ret2 == -1 || ret3 == -1 || ret4 == -1)
	{
		QMessageBox::warning(this, u8"错误", u8"保存失败，请检查网络连接！");
		return;
	}

	Sleep(20);
	//恢复阈值参数
	char hfBuf1[8];
	char hfBuf2[8];
	char hfBuf3[8];
	char hfBuf4[8];

	temp = ui.lineEdit_52->text().toInt();
	hfBuf1[0] = temp >> 8;
	hfBuf1[1] = temp & 0xff;
	temp = ui.lineEdit_53->text().toInt();
	hfBuf1[2] = temp >> 8;
	hfBuf1[3] = temp & 0xff;
	temp = 10 * ui.lineEdit_54->text().toFloat();
	hfBuf1[4] = temp >> 8;
	hfBuf1[5] = temp & 0xff;
	temp = 10 * ui.lineEdit_55->text().toFloat();
	hfBuf1[6] = temp >> 8;
	hfBuf1[7] = temp & 0xff;
	ret1 = dataCenter->sendDataToUdp(hfBuf1, 8, BMS::SaveHuifuPara1);

	temp = ui.lineEdit_56->text().toInt() + 40;
	hfBuf2[0] = temp >> 8;
	hfBuf2[1] = temp & 0xff;
	temp = ui.lineEdit_57->text().toInt() + 40;
	hfBuf2[2] = temp >> 8;
	hfBuf2[3] = temp & 0xff;
	temp = ui.lineEdit_58->text().toInt() + 40;
	hfBuf2[4] = temp >> 8;
	hfBuf2[5] = temp & 0xff;
	temp = ui.lineEdit_59->text().toInt() + 40;
	hfBuf2[6] = temp >> 8;
	hfBuf2[7] = temp & 0xff;
	ret2 = dataCenter->sendDataToUdp(hfBuf2, 8, BMS::SaveHuifuPara2);

	temp = 10 * ui.lineEdit_60->text().toFloat();
	hfBuf3[0] = temp >> 8;
	hfBuf3[1] = temp & 0xff;
	temp = 10 * ui.lineEdit_61->text().toFloat();
	hfBuf3[2] = temp >> 8;
	hfBuf3[3] = temp & 0xff;
	temp = ui.lineEdit_62->text().toInt();
	hfBuf3[4] = temp >> 8;
	hfBuf3[5] = temp & 0xff;
	temp = ui.lineEdit_63->text().toInt();
	hfBuf3[6] = temp >> 8;
	hfBuf3[7] = temp & 0xff;
	ret3 = dataCenter->sendDataToUdp(hfBuf3, 8, BMS::SaveHuifuPara3);

	temp = ui.lineEdit_64->text().toInt();
	hfBuf4[0] = temp >> 8;
	hfBuf4[1] = temp & 0xff;
	temp = ui.lineEdit_65->text().toInt();
	hfBuf4[2] = temp >> 8;
	hfBuf4[3] = temp & 0xff;
	hfBuf4[4] = 0;
	hfBuf4[5] = 0;
	hfBuf4[6] = 0;
	hfBuf4[7] = 0;
	ret4 = dataCenter->sendDataToUdp(hfBuf4, 8, BMS::SaveHuifuPara4);

	if (ret1 == -1 || ret2 == -1 || ret3 == -1 || ret4 == -1)
	{
		QMessageBox::warning(this, u8"错误", u8"保存失败，请检查网络连接！");
		return;
	}

}

//继电器控制事件
void MainWindow::confirmBtn_clicked()
{
	char buf[8];
	buf[0] = 2;
	buf[1] = 0;
	switch (ui.comboBox_2->currentIndex())
	{
	case 0:
		buf[2] = 0x11;
		break;
	case 1:
		buf[2] = 0x22;
		break;
	case 2:
		buf[2] = 0x33;
		break;
	default:
		break;
	}

	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;
	buf[6] = 0;
	buf[7] = 0;

	int ret = dataCenter->sendDataToUdp(buf, 8, BMS::SendCommand);
	if (ret != -1)
		QMessageBox::information(this, u8"提示", u8"操作成功!");
	else
		QMessageBox::warning(this, u8"错误", u8"操作失败!");
}

//读取电压校准参数
void MainWindow::readVolCal_clicked()
{
	//清空显示
	ui.lineEdit_5->setText("");
	ui.lineEdit_6->setText("");

	char buf[8];
	buf[0] = 1;
	buf[1] = 2;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;
	buf[6] = 0;
	buf[7] = 0;

	int ret = dataCenter->sendDataToUdp(buf, 8, BMS::SendCommand);
}

//保存电压校准参数
void MainWindow::saveVolCal_clicked()
{
	char buf[8];
	quint16 temp;

	temp = ui.lineEdit_5->text().toInt();
	buf[0] = temp >> 8;
	buf[1] = temp & 0xff;

	temp = 1000 * ui.lineEdit_6->text().toFloat();
	buf[2] = temp >> 8;
	buf[3] = temp & 0xff;

	buf[4] = 0;
	buf[5] = 0;
	buf[6] = 0;
	buf[7] = 0;

	int ret = dataCenter->sendDataToUdp(buf, 8, BMS::SaveVolCal);
}

//读取电流校准参数
void MainWindow::readCurCalBtn_clicked()
{
	//清空显示
	ui.lineEdit_11->setText("");
	ui.lineEdit_12->setText("");

	char buf[8];
	buf[0] = 1;
	buf[1] = 3;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;
	buf[6] = 0;
	buf[7] = 0;

	int ret = dataCenter->sendDataToUdp(buf, 8, BMS::SendCommand);
}

//保存电流校准参数
void MainWindow::saveCurCalBtn_clicked()
{
	char buf[8];
	quint16 temp;

	temp = 10 * ui.lineEdit_11->text().toFloat();
	buf[0] = temp >> 8;
	buf[1] = temp & 0xff;

	temp = 1000 * ui.lineEdit_6->text().toFloat();
	buf[2] = temp >> 8;
	buf[3] = temp & 0xff;

	buf[4] = 0;
	buf[5] = 0;
	buf[6] = 0;
	buf[7] = 0;

	int ret = dataCenter->sendDataToUdp(buf, 8, BMS::SaveCurCal);
}

//均衡功能状态开启
void MainWindow::equalStartBtn_clicked()
{
	char buf[8];
	buf[0] = 1;
	for (int i = 1; i < 8; i++)
		buf[i] = 0;
	int ret = dataCenter->sendDataToUdp(buf, 8, BMS::SendEqualFunCtrl);
	if (ret == -1)
		QMessageBox::warning(this, u8"错误", u8"开启失败！请检查网络连接！");
}

//均衡功能状态关闭
void MainWindow::equalCloseBtn_clicked()
{
	quint8 ps = 0xBA;
	char buf[8];

	for (int i = 0; i < 8; i++)
		buf[i] = 0;

	int ret = dataCenter->sendDataToUdp(buf, 8, BMS::SendEqualFunCtrl);
	if (ret == -1)
		QMessageBox::warning(this, u8"错误", u8"关闭失败！请检查网络连接！");
}

//暂停显示事件
void MainWindow::on_pauseBtn_clicked()
{
	if (ui.pauseBtn->text() == u8"暂停显示")
	{
		disconnect(dataCenter, &DataCenter::receiveOriginalData, this, &MainWindow::displayReceiveData);
		disconnect(dataCenter, &DataCenter::sendOriginalData, this, &MainWindow::displaySendData);
		ui.pauseBtn->setText(u8"继续显示");
	}
	else if (ui.pauseBtn->text() == u8"继续显示")
	{
		connect(dataCenter, &DataCenter::receiveOriginalData, this, &MainWindow::displayReceiveData);
		connect(dataCenter, &DataCenter::sendOriginalData, this, &MainWindow::displaySendData);
		ui.pauseBtn->setText(u8"暂停显示");
	}
}

//清空显示事件
void MainWindow::on_clearBtn_clicked()
{
	ui.textBrowser->clear();
	ui.lcdNumber->display(0);
	ui.lcdNumber_2->display(0);
	m_receFps = 0;
	m_sendFPs = 0;
}

//手动发送事件
void MainWindow::on_manualSendBtn_clicked()
{
	char ch;
	bool flag = false;
	uint32_t i, len;

	QString SendTextEditStr = ui.lineEdit->text();

	//去掉无用符号
	SendTextEditStr = SendTextEditStr.replace(' ', "");
	SendTextEditStr = SendTextEditStr.replace(',', "");
	SendTextEditStr = SendTextEditStr.replace('\r', "");
	SendTextEditStr = SendTextEditStr.replace('\n', "");
	SendTextEditStr = SendTextEditStr.replace('\t', "");
	SendTextEditStr = SendTextEditStr.replace("0x", "");
	SendTextEditStr = SendTextEditStr.replace("0X", "");
	//判断数据合法性
	for (i = 0, len = SendTextEditStr.length(); i < len; i++)
	{
		ch = SendTextEditStr.at(i).toLatin1();
		if (ch >= '0' && ch <= '9') {
			flag = false;
		}
		else if (ch >= 'a' && ch <= 'f') {
			flag = false;
		}
		else if (ch >= 'A' && ch <= 'F') {
			flag = false;
		}
		else {
			flag = true;
		}
	}
	if (flag)
	{
		QMessageBox::warning(this, u8"警告", u8"输入内容包含非法16进制字符！");
		return;
	}

	//QString转QByteArray
	QByteArray SendTextEditBa = QString2Hex(SendTextEditStr);
	int size = SendTextEditBa.size();
	char* data;
	data = SendTextEditBa.data();
	//memcpy(data, SendTextEditBa, size);
	int n = sizeof(data);
	int ret = dataCenter->manualSendToUdp(data, size);
	if (ret == -1)
		QMessageBox::warning(this, u8"错误", u8"请检查网络连接！");
}

//将一个字符串转换成十六进制
QByteArray MainWindow::QString2Hex(QString str)
{
	QByteArray senddata;
	int hexdata, lowhexdata;
	int hexdatalen = 0;
	int len = str.length();
	senddata.resize(len / 2);
	char lstr, hstr;
	for (int i = 0; i<len; )
	{
		hstr = str[i].toLatin1();
		if (hstr == ' ')
		{
			i++;
			continue;
		}
		i++;
		if (i >= len)
			break;
		lstr = str[i].toLatin1();
		hexdata = ConvertHexChar(hstr);
		lowhexdata = ConvertHexChar(lstr);
		if ((hexdata == 16) || (lowhexdata == 16))
			break;
		else
			hexdata = hexdata * 16 + lowhexdata;
		i++;
		senddata[hexdatalen] = (char)hexdata;
		hexdatalen++;
	}
	senddata.resize(hexdatalen);
	return senddata;
}

char MainWindow::ConvertHexChar(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - 0x30;
	else if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	else if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else return (-1);
}

//刷新第一页
void MainWindow::on_updateBtn_clicked()
{
	ui.lineEdit_totalV->setText("");
	ui.lineEdit_totalA->setText("");
	ui.lineEdit_aveV->setText("");
	ui.lineEdit_aveT->setText("");
	ui.lineEdit_SOC->setText("");
	ui.lineEdit_SOH->setText("");
	ui.lineEdit_chargeCutOffTotalV->setText("");
	ui.lineEdit_dischargeCutOffTotalV->setText("");
	ui.lineEdit_maxAllowedChargeA->setText("");
	ui.lineEdit_maxAllowedDischargeA->setText("");
	ui.lineEdit_maxV->setText("");
	ui.lineEdit_maxVNo->setText("");
	ui.lineEdit_minV->setText("");
	ui.lineEdit_minVNo->setText("");
	ui.lineEdit_vDeff->setText("");
	ui.lineEdit_maxT->setText("");
	ui.lineEdit_MaxTNo->setText("");
	ui.lineEdit_minT->setText("");
	ui.lineEdit_minTNo->setText("");
	ui.lineEdit_TDeff->setText("");
	ui.lineEdit_batStatet->setText("");
	ui.lineEdit_relayState->setText("");

	ui.label_cellQainYa->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_cellGuoYa->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_packQianYa->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_packGuoYa->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_chDiWen->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_disDiWen->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_chGaoWen->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_disGaoWen->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_chGuoLiu->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_disGuoLiu->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_tongXinGuZhang->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_jueYuanGuzhang->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_yaChaGuoDa->setPixmap(QPixmap(u8":/images/正常.ico"));
	ui.label_wenChaGuoDa->setPixmap(QPixmap(u8":/images/正常.ico"));
}