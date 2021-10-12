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
			QMessageBox::information(this, u8"��ʾ", u8"UDP�򿪳ɹ���");
		}
		else {
			QMessageBox::warning(this, u8"����", u8"UDP��ʧ�ܣ�");
		}
	}
	delete dlg;
	dlg = NULL;
}

void MainWindow::on_actioncloseudp_triggered()
{
	dataCenter->udpSocket->close();
	QMessageBox::information(this, u8"��ʾ", u8"UDP�رճɹ���");
}

//�������Ĵ���
void MainWindow::dataCenterError(QString str)
{
}

//����������Ϣ
void MainWindow::messageFromDataCenter(QString str)
{
}

// ��������
void MainWindow::dataChange(DataCenter::DataType type)
{
	switch (type)
	{
	case DataCenter::BatPackStat:
		BMS::BatPackStatDataSt batPack = dataCenter->getBatPackStatData();
		/*�ڽ�����ʾ��Ϣ*/
		ui.lineEdit_totalV->setText(QString::number(batPack.totalVol));
		ui.lineEdit_totalA->setText(QString::number(batPack.totalCur));
		ui.lineEdit_aveV->setText(QString::number(batPack.aveVol));
		ui.lineEdit_vDeff->setText(QString::number(batPack.volDeff));
		break;
	case DataCenter::VolMaxMin:
		BMS::VolMaxMinDataSt volMaxMin = dataCenter->getVolMaxMinData();
		/*������ʾ��Ϣ*/
		ui.lineEdit_maxV->setText(QString::number(volMaxMin.maxVol));
		ui.lineEdit_maxVNo->setText(QString::number(volMaxMin.maxNo));
		ui.lineEdit_minV->setText(QString::number(volMaxMin.minVol));
		ui.lineEdit_minVNo->setText(QString::number(volMaxMin.minNo));
		break;
	case DataCenter::TempMaxMin:
		BMS::TempMaxMinDataSt tempMaxMinData = dataCenter->getTempMaxMinData();
		/*������ʾ��Ϣ*/
		ui.lineEdit_maxT->setText(QString::number(tempMaxMinData.tempMax));
		ui.lineEdit_minT->setText(QString::number(tempMaxMinData.tempMin));
		ui.lineEdit_MaxTNo->setText(QString::number(tempMaxMinData.maxNo));
		ui.lineEdit_minTNo->setText(QString::number(tempMaxMinData.minNo));
		ui.lineEdit_TDeff->setText(QString::number(tempMaxMinData.tempDeff));
		ui.lineEdit_aveT->setText(QString::number(tempMaxMinData.aveTemp));
		break;
	case DataCenter::CharDisCutOff:
		BMS::CharDisCutOffDataSt charDisCutOffData = dataCenter->getCharDisCutOffData();
		/*������ʾ��Ϣ*/
		ui.lineEdit_chargeCutOffTotalV->setText(QString::number(charDisCutOffData.charCutOffTotalVol));
		ui.lineEdit_dischargeCutOffTotalV->setText(QString::number(charDisCutOffData.disCutOffTotalVol));
		ui.lineEdit_maxAllowedChargeA->setText(QString::number(charDisCutOffData.maxAllowedCharCur));
		ui.lineEdit_maxAllowedDischargeA->setText(QString::number(charDisCutOffData.maxAllowedDisCur));
		break;
	case DataCenter::SOCSOH:
		BMS::SOCSOHDataSt SOCSOHData = dataCenter->getSOCSOHData();
		/*������ʾ��Ϣ*/
		ui.lineEdit_SOC->setText(QString::number(SOCSOHData.soc));
		ui.lineEdit_SOH->setText(QString::number(SOCSOHData.soh));
	case DataCenter::BMSState:
		BMS::BMSStateDataSt	BMSStateData = dataCenter->getBMSStateData();
		/*������ʾ��Ϣ*/
		switch (BMSStateData.batState)//���״̬
		{
		case 0:
			ui.lineEdit_batStatet->setText(u8"����");
			break;
		case 1:
			ui.lineEdit_batStatet->setText(u8"���");
			break;
		case 2:
			ui.lineEdit_batStatet->setText(u8"f�ŵ�");
			break;
		default:
			break;
		}
		switch (BMSStateData.relayState)//�̵���״̬
		{
		case 0:
			ui.lineEdit_relayState->setText(u8"�Ͽ�");
			break;
		case 1:
			ui.lineEdit_relayState->setText(u8"�պ�");
		}
	}
}