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
	/*�����ѹ���*/
	QStringList header;
	header << u8"���" << u8"��ѹ(V)" << u8"����״̬";
	ui.tableWidget->setHorizontalHeaderLabels(header);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);//����ͷ
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//�������
	ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");//��ͷ��ʽ
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//���ɱ༭

	/*�����¶ȱ��*/
	header.clear();
	header << u8"���" << u8"�¶�(��)";
	ui.tableWidget_2->setHorizontalHeaderLabels(header);
	ui.tableWidget_2->horizontalHeader()->setStretchLastSection(true);//����ͷ
	ui.tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//�������
	ui.tableWidget_2->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");//��ͷ��ʽ
	ui.tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);//���ɱ༭

	//ˢ������
	QIntValidator* IntValidator = new QIntValidator(this);
	IntValidator->setRange(1, 60);//���Ըĳɣ�-255,255�����������Ǹ���
	ui.periodLineEdit->setValidator(IntValidator);
	ui.periodLineEdit->setPlaceholderText(u8"������1-60");//������ʾ�û����뷶Χ
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
	case DataCenter::BatTempNum:
		BMS::BatTempNumDataSt batTempNum = dataCenter->getBatTempNumData();
		m_batTotalNum = batTempNum.batNum;
		m_tempTotalNum = batTempNum.tempNum;

		//ʵ��������
		m_volVec.resize(m_batTotalNum);
		m_tempVec.resize(m_tempTotalNum);

		//Ԥ�ñ��
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
	case DataCenter::CellVol://�����ѹ
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
	case DataCenter::CellTemp://�����¶�
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
	case DataCenter::EqualState://����״̬
		BMS::EqualStateDataSt equalState = dataCenter->getEqualStateData();
		for (int i = 0; i < 4; i++)
		{
			equalStateVec.push_back(equalState.equalType[i]);
			equalNoVec.push_back(equalState.equalNo[i]);
		}
	case DataCenter::BatPackStat://�����״̬
		BMS::BatPackStatDataSt batPack = dataCenter->getBatPackStatData();
		/*�ڽ�����ʾ��Ϣ*/
		ui.lineEdit_totalV->setText(QString::number(batPack.totalVol));
		ui.lineEdit_totalA->setText(QString::number(batPack.totalCur));
		ui.lineEdit_aveV->setText(QString::number(batPack.aveVol));
		ui.lineEdit_vDeff->setText(QString::number(batPack.volDeff));
		break;
	case DataCenter::VolMaxMin://��ѹ��ֵ
		BMS::VolMaxMinDataSt volMaxMin = dataCenter->getVolMaxMinData();
		/*������ʾ��Ϣ*/
		ui.lineEdit_maxV->setText(QString::number(volMaxMin.maxVol));
		ui.lineEdit_maxVNo->setText(QString::number(volMaxMin.maxNo));
		ui.lineEdit_minV->setText(QString::number(volMaxMin.minVol));
		ui.lineEdit_minVNo->setText(QString::number(volMaxMin.minNo));
		break;
	case DataCenter::TempMaxMin://�¶���ֵ
		BMS::TempMaxMinDataSt tempMaxMinData = dataCenter->getTempMaxMinData();
		/*������ʾ��Ϣ*/
		ui.lineEdit_maxT->setText(QString::number(tempMaxMinData.tempMax));
		ui.lineEdit_minT->setText(QString::number(tempMaxMinData.tempMin));
		ui.lineEdit_MaxTNo->setText(QString::number(tempMaxMinData.maxNo));
		ui.lineEdit_minTNo->setText(QString::number(tempMaxMinData.minNo));
		ui.lineEdit_TDeff->setText(QString::number(tempMaxMinData.tempDeff));
		ui.lineEdit_aveT->setText(QString::number(tempMaxMinData.aveTemp));
		break;
	case DataCenter::CharDisCutOff://��ŵ��ֹ
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
		break;
	case DataCenter::BMSState://BMS״̬
	{
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
			break;
		default:
			break;
		}

		/*һ������*/
		if (BMSStateData.fetalErr & BMS::ERR_CELL_UV)//�����ѹǷѹ
			ui.label_cellQainYa->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CELL_OV)//�����ѹ��ѹ
			ui.label_cellGuoYa->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_PACK_UV)//�ܵ�ѹǷѹ
			ui.label_packQianYa->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_PACK_OV)//�ܵ�ѹ��ѹ
			ui.label_packGuoYa->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CHR_UT)//����¶ȹ���
			ui.label_chDiWen->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CHR_OT)//����¶ȹ���
			ui.label_chGaoWen->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_DISCH_UT)//�ŵ��¶ȹ���
			ui.label_disDiWen->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_DISCH_OT)//�ŵ��¶ȹ���
			ui.label_disGaoWen->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_CHR_OC)//������
			ui.label_chGuoLiu->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_DISCH_OC)//�ŵ����
			ui.label_disGuoLiu->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_BMU_COMM)//BMUͨ���쳣
			ui.label_tongXinGuZhang->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_INS_LOW)//��Ե����
			ui.label_jueYuanGuzhang->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_VOLT_DIFF)//ѹ�����
			ui.label_yaChaGuoDa->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.fetalErr & BMS::ERR_TEMP_DIFF)//�²����
			ui.label_wenChaGuoDa->setPixmap(QPixmap(":/images/����.ico"));

		/*��������*/
		if (BMSStateData.warningErr & BMS::ERR_CELL_UV)//�����ѹǷѹ
			ui.label_cellQainYa->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CELL_OV)//�����ѹ��ѹ
			ui.label_cellGuoYa->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_PACK_UV)//�ܵ�ѹǷѹ
			ui.label_packQianYa->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_PACK_OV)//�ܵ�ѹ��ѹ
			ui.label_packGuoYa->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CHR_UT)//����¶ȹ���
			ui.label_chDiWen->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CHR_OT)//����¶ȹ���
			ui.label_chGaoWen->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_DISCH_UT)//�ŵ��¶ȹ���
			ui.label_disDiWen->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_DISCH_OT)//�ŵ��¶ȹ���
			ui.label_disGaoWen->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_CHR_OC)//������
			ui.label_chGuoLiu->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_DISCH_OC)//�ŵ����
			ui.label_disGuoLiu->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_BMU_COMM)//BMUͨ���쳣
			ui.label_tongXinGuZhang->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_INS_LOW)//��Ե����
			ui.label_jueYuanGuzhang->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_VOLT_DIFF)//ѹ�����
			ui.label_yaChaGuoDa->setPixmap(QPixmap(":/images/����.ico"));
		if (BMSStateData.warningErr & BMS::ERR_TEMP_DIFF)//�²����
			ui.label_wenChaGuoDa->setPixmap(QPixmap(":/images/����.ico"));

		/*����*/
		quint16 ERR_FLAG = BMSStateData.fetalErr | BMSStateData.warningErr;
		ERR_FLAG = ~ERR_FLAG;//Ϊ1��λΪ����
		if (ERR_FLAG & BMS::ERR_CELL_UV)//�����ѹǷѹ
			ui.label_cellQainYa->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_CELL_OV)//�����ѹ��ѹ
			ui.label_cellGuoYa->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_PACK_UV)//�ܵ�ѹǷѹ
			ui.label_packQianYa->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_PACK_OV)//�ܵ�ѹ��ѹ
			ui.label_packGuoYa->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_CHR_UT)//����¶ȹ���
			ui.label_chDiWen->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_CHR_OT)//����¶ȹ���
			ui.label_chGaoWen->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_DISCH_UT)//�ŵ��¶ȹ���
			ui.label_disDiWen->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_DISCH_OT)//�ŵ��¶ȹ���
			ui.label_disGaoWen->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_CHR_OC)//������
			ui.label_chGuoLiu->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_DISCH_OC)//�ŵ����
			ui.label_disGuoLiu->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_BMU_COMM)//BMUͨ���쳣
			ui.label_tongXinGuZhang->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_INS_LOW)//��Ե����
			ui.label_jueYuanGuzhang->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_VOLT_DIFF)//ѹ�����
			ui.label_yaChaGuoDa->setPixmap(QPixmap(":/images/����.ico"));
		if (ERR_FLAG & BMS::ERR_TEMP_DIFF)//�²����
			ui.label_wenChaGuoDa->setPixmap(QPixmap(":/images/����.ico"));
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

//ˢ�±��
void MainWindow::on_dataFlushBtn_clicked()
{
	//�����ѹ
	for (int i = 0; i < m_volVec.size(); i++)
	{
		QTableWidgetItem *item = new QTableWidgetItem();
		item->setText(QString::number(m_volVec[i]));
		ui.tableWidget->setItem(i, 1, item);
	}

	//����״̬
	for (int i = 0; i < equalStateVec.size(); i++)
	{
		QTableWidgetItem *item = new QTableWidgetItem();
		if (equalStateVec.at(i))//����
		{
			item->setText(u8"��������");
		}
		else {//����
			item->setText(u8"��������");
		}
		ui.tableWidget->setItem(equalNoVec.at(i), 2, item);
	}

	//�����¶�
	for (int i = 0; i < m_tempVec.size(); i++)
	{
		QTableWidgetItem *item = new QTableWidgetItem();
		item->setText(QString::number(m_tempVec[i]));
		ui.tableWidget_2->setItem(i, 1, item);
	}
}

//�Զ�ˢ���¼�
void MainWindow::on_dataCheckBox_stateChanged(int state)
{
	if (state == Qt::Checked) // "ѡ��"
	{
		int interval = ui.periodLineEdit->text().toInt();
		if (interval > 0)
		{
			timer->setInterval(interval);
			timer->start();
		}
		ui.periodLineEdit->setReadOnly(true);
	} 
	else // δѡ�� - Qt::Unchecked
	{
		timer->stop();
		ui.periodLineEdit->setReadOnly(false);
	}
}

//����AD1
void MainWindow::on_volCalBtn_clicked()
{
	float ceH = ui.lineEdit_1->text().toFloat();
	float ceL = ui.lineEdit_2->text().toFloat();
	float zhenH = ui.lineEdit_3->text().toFloat();
	float zhenL = ui.lineEdit_4->text().toFloat();

	if (ceH == ceL)
	{
		QMessageBox::critical(this, u8"������ʾ", "value error!");
		return;
	}

	float gain = (zhenH - zhenL) / (ceH - ceL);
	float offset = ceL * gain - zhenL;

	ui.lineEdit_5->setText(QString::number(gain));
	ui.lineEdit_6->setText(QString::number(offset));
}