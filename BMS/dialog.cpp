#include "dialog.h"

Dialog::Dialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
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


Dialog::~Dialog()
{
}
