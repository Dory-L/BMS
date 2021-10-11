#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);

	udpSocket = new QUdpSocket(this);

	iniConnect();
	guiInitate();
}

void MainWindow::iniConnect()
{
	connect(ui.actionopenudp, SIGNAL(triggered), this, SLOT(on_actionopenudp_triggered));
	connect(ui.actioncloseudp, SIGNAL(triggered), this, SLOT(on_actioncloseudp_triggered));

	//dataCenter
	/*connect(dataCenter, &DataCenter::error, this, &MainWindow::dataCenterError);
	connect(dataCenter, &DataCenter::newData, this, &MainWindow::dataChange);
	connect(dataCenter, &DataCenter::newMassage, this, &MainWindow::messageFromDataCenter);
	connect(dataCenter, &DataCenter::warning, this, &MainWindow::messageFromDataCenter);*/

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

		if ((udpSocket->bind(dlg->getLocalPort()) == true))
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
	udpSocket->close();
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
}