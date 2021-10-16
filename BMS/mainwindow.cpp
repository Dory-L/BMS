#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	dataCenter = new DataCenter(this);
	//dataCenter->udpSocket = new QUdpSocket(this);

	timer = new QTimer(this);

	iniConnect();
	guiInitate();
}

void MainWindow::iniConnect()
{
	connect(ui.actionopenudp, SIGNAL(triggered), this, SLOT(on_actionopenudp_triggered));
	connect(ui.actioncloseudp, SIGNAL(triggered), this, SLOT(on_actioncloseudp_triggered));
	connect(timer, SIGNAL(timeout), this, SLOT(on_dataCheckBox_stateChanged));
	//dataCenter
	connect(dataCenter, &DataCenter::newData, this, &MainWindow::dataChange);
}

void MainWindow::guiInitate()
{ 
	/*单体电压表格*/
	QStringList header;
	header << u8"序号" << u8"电压(V)" << u8"均衡状态";
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

	//刷新设置
	QIntValidator* IntValidator = new QIntValidator(this);
	IntValidator->setRange(1, 60);//可以改成（-255,255），这样就是负数
	ui.periodLineEdit->setValidator(IntValidator);
	ui.periodLineEdit->setPlaceholderText(u8"请输入1-60");//背景提示用户输入范围
}

void MainWindow::on_actionopenudp_triggered()
{
	dlg = new Dialog();
	int ret = dlg->exec();
	if (ret == QDialog::Accepted)
	{
		aimIp = dlg->getAimIp();
		aimPort = dlg->getAimPort();

		int localPort = dlg->getLocalPort();

		if ((dataCenter->udpSocket->bind(localPort) == true))
		{
			QMessageBox::information(this, u8"提示", u8"UDP打开成功！");
		}
		else {
			QMessageBox::warning(this, u8"警告", u8"UDP打开失败！");
		}
	}
	delete dlg;
	dlg = NULL;
}

void MainWindow::on_actioncloseudp_triggered()
{
	dataCenter->udpSocket->close();
	QMessageBox::information(this, u8"提示", u8"UDP关闭成功！");
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
	case DataCenter::BatTempNum:
		BMS::BatTempNumDataSt batTempNum = dataCenter->getBatTempNumData();
		m_batTotalNum = batTempNum.batNum;
		m_tempTotalNum = batTempNum.tempNum;

		//实例化数组
		m_volVec.resize(m_batTotalNum);
		m_tempVec.resize(m_tempTotalNum);

		//预置表格
		ui.tableWidget->setRowCount(m_batTotalNum);
		for (int i = 0; i < m_batTotalNum; i++)
		{
			QTableWidgetItem *number = new QTableWidgetItem();
			number->setText(QString::number(i + 1));
			ui.tableWidget->setItem(i, 0, number);
		}

		ui.tableWidget_2->setRowCount(m_tempTotalNum);
		for (int i = 0; i < m_tempTotalNum; i++)
		{
			QTableWidgetItem *number = new QTableWidgetItem();
			number->setText(QString::number(i + 1));
			ui.tableWidget->setItem(i, 0, number);
		}
		break;
	case DataCenter::CellVol://单体电压
		BMS::CellVolDataSt cellVol = dataCenter->getCellVolData();
		for (int i = 0; i < 4; i++)
		{
			if (cellVol.frameNo * 4 + i + 1 <= m_batTotalNum)
			{
				m_volVec[cellVol.frameNo * 4 + i] = cellVol.vol[i];
				m_volCount++;

				if (m_volCount == m_batTotalNum)
				{
					m_volVecComplete.swap(m_volVec);
					m_volCount = 0;
				}
			}
		}
	case DataCenter::CellTemp://单体温度
		BMS::CellTempDataSt cellTemp = dataCenter->getCellTempData();
		for (int i = 0; i < 4; i++)
		{
			if (cellTemp.frameNo * 4 + i + 1 <= m_tempTotalNum)
			{
				m_tempVec[cellVol.frameNo * 4 + i] = cellTemp.temp[i];
				m_tempCount++;

				if (m_tempCount == m_tempTotalNum)
				{
					m_tempVecComplete.swap(m_tempVec);
					m_tempCount = 0;
				}
			}
		}
	case DataCenter::EqualState://均衡状态
		BMS::EqualStateDataSt equalState = dataCenter->getEqualStateData();
		for (int i = 0; i < 4; i++)
		{
			equalStateVec.push_back(equalState.equalType[i]);
			equalNoVec.push_back(equalState.equalNo[i]);
		}
	case DataCenter::BatPackStat://电池组状态
		BMS::BatPackStatDataSt batPack = dataCenter->getBatPackStatData();
		/*在界面显示信息*/
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

		/*一级保护*/
		if (BMSStateData.fetalErr & BMS::ERR_CELL_UV)//单体电压欠压
			ui.label_cellQainYa->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CELL_OV)//单体电压过压
			ui.label_cellGuoYa->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_PACK_UV)//总电压欠压
			ui.label_packQianYa->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_PACK_OV)//总电压过压
			ui.label_packGuoYa->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CHR_UT)//充电温度过低
			ui.label_chDiWen->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CHR_OT)//充电温度过高
			ui.label_chGaoWen->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_DISCH_UT)//放电温度过低
			ui.label_disDiWen->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_DISCH_OT)//放电温度过高
			ui.label_disGaoWen->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CHR_OC)//充电过流
			ui.label_chGuoLiu->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_DISCH_OC)//放电过流
			ui.label_disGuoLiu->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_BMU_COMM)//BMU通信异常
			ui.label_tongXinGuZhang->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_INS_LOW)//绝缘过低
			ui.label_jueYuanGuzhang->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_VOLT_DIFF)//压差过大
			ui.label_yaChaGuoDa->setPixmap(QPixmap(":/images/保护.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_TEMP_DIFF)//温差过大
			ui.label_wenChaGuoDa->setPixmap(QPixmap(":/images/保护.ico"));

		/*二级保护*/
		if (BMSStateData.warningErr & BMS::ERR_CELL_UV)//单体电压欠压
			ui.label_cellQainYa->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CELL_OV)//单体电压过压
			ui.label_cellGuoYa->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_PACK_UV)//总电压欠压
			ui.label_packQianYa->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_PACK_OV)//总电压过压
			ui.label_packGuoYa->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CHR_UT)//充电温度过低
			ui.label_chDiWen->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CHR_OT)//充电温度过高
			ui.label_chGaoWen->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_DISCH_UT)//放电温度过低
			ui.label_disDiWen->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_DISCH_OT)//放电温度过高
			ui.label_disGaoWen->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CHR_OC)//充电过流
			ui.label_chGuoLiu->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_DISCH_OC)//放电过流
			ui.label_disGuoLiu->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_BMU_COMM)//BMU通信异常
			ui.label_tongXinGuZhang->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_INS_LOW)//绝缘过低
			ui.label_jueYuanGuzhang->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_VOLT_DIFF)//压差过大
			ui.label_yaChaGuoDa->setPixmap(QPixmap(":/images/报警.ico"));
		if (BMSStateData.warningErr & BMS::ERR_TEMP_DIFF)//温差过大
			ui.label_wenChaGuoDa->setPixmap(QPixmap(":/images/报警.ico"));

		/*正常*/
		quint16 ERR_FLAG = BMSStateData.fetalErr | BMSStateData.warningErr;
		ERR_FLAG = ~ERR_FLAG;//为1的位为正常
		if (ERR_FLAG & BMS::ERR_CELL_UV)//单体电压欠压
			ui.label_cellQainYa->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_CELL_OV)//单体电压过压
			ui.label_cellGuoYa->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_PACK_UV)//总电压欠压
			ui.label_packQianYa->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_PACK_OV)//总电压过压
			ui.label_packGuoYa->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_CHR_UT)//充电温度过低
			ui.label_chDiWen->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_CHR_OT)//充电温度过高
			ui.label_chGaoWen->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_DISCH_UT)//放电温度过低
			ui.label_disDiWen->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_DISCH_OT)//放电温度过高
			ui.label_disGaoWen->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_CHR_OC)//充电过流
			ui.label_chGuoLiu->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_DISCH_OC)//放电过流
			ui.label_disGuoLiu->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_BMU_COMM)//BMU通信异常
			ui.label_tongXinGuZhang->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_INS_LOW)//绝缘过低
			ui.label_jueYuanGuzhang->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_VOLT_DIFF)//压差过大
			ui.label_yaChaGuoDa->setPixmap(QPixmap(":/images/正常.ico"));
		if (ERR_FLAG & BMS::ERR_TEMP_DIFF)//温差过大
			ui.label_wenChaGuoDa->setPixmap(QPixmap(":/images/正常.ico"));
		break;
	}
	case DataCenter::BatPackConf1:
		BMS::BatPackConfData1St	batPackConfData1 = dataCenter->getBatPackConfData1();

		ui.lineEdit_13->setText(QString::number(batPackConfData1.batDesignCapacity));
		ui.lineEdit_14->setText(QString::number(batPackConfData1.bmuNum));
		ui.lineEdit_15->setText(QString::number(batPackConfData1.cellBatNum));
		break;
	case DataCenter::BatPackConf2:
		BMS::BatPackConfData2St	batPackConfData2 = dataCenter->getBatPackConfData2();

		ui.lineEdit_16->setText(QString::number(batPackConfData2.tempSensorNum));
		ui.lineEdit_17->setText(QString::number(batPackConfData2.endSlaveBatNum));
		ui.lineEdit_18->setText(QString::number(batPackConfData2.endSlaveTempNum));
		ui.lineEdit_19->setText(QString::number(batPackConfData2.batMaxCapacity));
		break;
	case DataCenter::BatPackConf3:
		BMS::BatPackConfData3St	batPackConfData3 = dataCenter->getBatPackConfData3();

		ui.lineEdit_20->setText(QString::number(batPackConfData3.batCurCapacity));
		ui.lineEdit_21->setText(QString::number(batPackConfData3.batCurLeftCapacity));
		ui.lineEdit_22->setText(QString::number(batPackConfData3.equalStartPressure));
		ui.lineEdit_23->setText(QString::number(batPackConfData3.equalClosePressure));
		break;
	}
}

//刷新表格
void MainWindow::on_dataFlushBtn_clicked()
{
	//单体电压
	for (int i = 0; i < m_volVec.size(); i++)
	{
		QTableWidgetItem *item = new QTableWidgetItem();
		item->setText(QString::number(m_volVec[i]));
		ui.tableWidget->setItem(i, 1, item);
	}

	//均衡状态
	for (int i = 0; i < equalStateVec.size(); i++)
	{
		QTableWidgetItem *item = new QTableWidgetItem();
		if (equalStateVec.at(i))//主动
		{
			item->setText(u8"主动均衡");
		}
		else {//被动
			item->setText(u8"被动均衡");
		}
		ui.tableWidget->setItem(equalNoVec.at(i), 2, item);
	}

	//单体温度
	for (int i = 0; i < m_tempVec.size(); i++)
	{
		QTableWidgetItem *item = new QTableWidgetItem();
		item->setText(QString::number(m_tempVec[i]));
		ui.tableWidget_2->setItem(i, 1, item);
	}
}

//自动刷新事件
void MainWindow::on_dataCheckBox_stateChanged(int state)
{
	if (state == Qt::Checked) // "选中"
	{
		int interval = ui.periodLineEdit->text().toInt();
		if (interval > 0)
		{
			timer->setInterval(interval);
			timer->start();
		}
		ui.periodLineEdit->setReadOnly(true);
	} 
	else // 未选中 - Qt::Unchecked
	{
		timer->stop();
		ui.periodLineEdit->setReadOnly(false);
	}
}

//计算AD1
void MainWindow::on_volCalBtn_clicked()
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

	ui.lineEdit_5->setText(QString::number(gain));
	ui.lineEdit_6->setText(QString::number(offset));
}