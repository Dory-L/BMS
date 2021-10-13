#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	dataCenter = new DataCenter(this);
	//dataCenter->udpSocket = new QUdpSocket(this);

	iniConnect();
	guiInitate();
}

void MainWindow::iniConnect()
{
	connect(ui.actionopenudp, SIGNAL(triggered), this, SLOT(on_actionopenudp_triggered));
	connect(ui.actioncloseudp, SIGNAL(triggered), this, SLOT(on_actioncloseudp_triggered));

	//dataCenter
	connect(dataCenter, &DataCenter::newData, this, &MainWindow::dataChange);
}

void MainWindow::guiInitate()
{
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
	case DataCenter::BatPackStat:
		BMS::BatPackStatDataSt batPack = dataCenter->getBatPackStatData();
		/*在界面显示信息*/
		ui.lineEdit_totalV->setText(QString::number(batPack.totalVol));
		ui.lineEdit_totalA->setText(QString::number(batPack.totalCur));
		ui.lineEdit_aveV->setText(QString::number(batPack.aveVol));
		ui.lineEdit_vDeff->setText(QString::number(batPack.volDeff));
		break;
	case DataCenter::VolMaxMin:
		BMS::VolMaxMinDataSt volMaxMin = dataCenter->getVolMaxMinData();
		/*界面显示信息*/
		ui.lineEdit_maxV->setText(QString::number(volMaxMin.maxVol));
		ui.lineEdit_maxVNo->setText(QString::number(volMaxMin.maxNo));
		ui.lineEdit_minV->setText(QString::number(volMaxMin.minVol));
		ui.lineEdit_minVNo->setText(QString::number(volMaxMin.minNo));
		break;
	case DataCenter::TempMaxMin:
		BMS::TempMaxMinDataSt tempMaxMinData = dataCenter->getTempMaxMinData();
		/*界面显示信息*/
		ui.lineEdit_maxT->setText(QString::number(tempMaxMinData.tempMax));
		ui.lineEdit_minT->setText(QString::number(tempMaxMinData.tempMin));
		ui.lineEdit_MaxTNo->setText(QString::number(tempMaxMinData.maxNo));
		ui.lineEdit_minTNo->setText(QString::number(tempMaxMinData.minNo));
		ui.lineEdit_TDeff->setText(QString::number(tempMaxMinData.tempDeff));
		ui.lineEdit_aveT->setText(QString::number(tempMaxMinData.aveTemp));
		break;
	case DataCenter::CharDisCutOff:
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
	case DataCenter::BMSState:
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
		}

		switch (BMSStateData.fetalErr)//一级保护标志
		{
		case BMS::ERR_CELL_UV://单体电压欠压
			ui.label_cellQainYa->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_CELL_OV://单体电压过压
			ui.label_cellGuoYa->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_PACK_UV://总电压欠压
			ui.label_packQianYa->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_PACK_OV://总电压过压
			ui.label_packGuoYa->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_CHR_UT://充电温度过低
			ui.label_chDiWen->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_CHR_OT://充电温度过高
			ui.label_chGaoWen->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_DISCH_UT://放电温度过低
			ui.label_disDiWen->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_DISCH_OT://放电温度过高
			ui.label_disGaoWen->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_CHR_OC://充电过流
			ui.label_chGuoLiu->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_DISCH_OC://放电过流
			ui.label_disGuoLiu->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_BMU_COMM://BMU通信异常
			ui.label_tongXinGuZhang->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_INS_LOW://绝缘过低
			ui.label_jueYuanGuzhang->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_VOLT_DIFF://压差过大
			ui.label_yaChaGuoDa->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		case BMS::ERR_TEMP_DIFF://温差过大
			ui.label_wenChaGuoDa->setPixmap(QPixmap(":/images/保护.ico"));
			break;
		default:
			break;
		}

		switch (BMSStateData.warningErr)//二级保护标志
		{
		case BMS::ERR_CELL_UV://单体电压欠压
			ui.label_cellQainYa->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_CELL_OV://单体电压过压
			ui.label_cellGuoYa->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_PACK_UV://总电压欠压
			ui.label_packQianYa->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_PACK_OV://总电压过压
			ui.label_packGuoYa->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_CHR_UT://充电温度过低
			ui.label_chDiWen->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_CHR_OT://充电温度过高
			ui.label_chGaoWen->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_DISCH_UT://放电温度过低
			ui.label_disDiWen->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_DISCH_OT://放电温度过高
			ui.label_disGaoWen->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_CHR_OC://充电过流
			ui.label_chGuoLiu->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_DISCH_OC://放电过流
			ui.label_disGuoLiu->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_BMU_COMM://BMU通信异常
			ui.label_tongXinGuZhang->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_INS_LOW://绝缘过低
			ui.label_jueYuanGuzhang->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_VOLT_DIFF://压差过大
			ui.label_yaChaGuoDa->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		case BMS::ERR_TEMP_DIFF://温差过大
			ui.label_wenChaGuoDa->setPixmap(QPixmap(":/images/报警.ico"));
			break;
		default:
			break;
		}

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

	}
}