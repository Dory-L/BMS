#include "dialog.h"

Dialog::Dialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	QRegExp rx("((1?\\d{1,2}|2[0-5]{2})\\.){3}(1?\\d{1,2}|2[0-5]{2})");
	QValidator * validator = new QRegExpValidator(rx, this);
	ui.aimiplineEdit->setValidator(validator);
}

int Dialog::getLocalPort()
{
	return ui.localportlineEdit->text().toInt();
}

int Dialog::getAimPort()
{
	return ui.aimportlineEdit->text().toInt();
}

QString Dialog::getAimIp()
{
	return ui.aimiplineEdit->text();
}

void Dialog::on_pushButton_clicked()
{
	aimPort = ui.aimportlineEdit->text().toInt();
	aimIp = ui.aimiplineEdit->text();
	emit aimOk_Signal(aimPort, aimIp);
}
void Dialog::on_pushButton_2_clicked()
{
	localPort = ui.localportlineEdit->text().toInt();
	emit localOk_Signal(localPort);
}