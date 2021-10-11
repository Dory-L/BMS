#pragma once

#include <QDialog>
#include <QString>
#include "ui_dialog.h"

class Dialog : public QDialog
{
	Q_OBJECT

public:
	Dialog(QWidget *parent = Q_NULLPTR);
	~Dialog();

	int getLocalPort();
	int getAimPort();
	QString getAimIp();
	
private:
	Ui::Dialog ui;
	
};
