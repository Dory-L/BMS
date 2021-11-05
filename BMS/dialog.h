#pragma once

#include <QDialog>
#include <QString>
#include "ui_dialog.h"

class Dialog : public QDialog
{
	Q_OBJECT

public:
	Dialog(QWidget *parent = Q_NULLPTR);

	int getLocalPort();
	int getAimPort();
	QString getAimIp();
signals:
	void aimOk_Signal(int aimPort, QString aimIp);
	void localOk_Signal(int localPort);
private:
	Ui::Dialog ui;

	int aimPort;
	QString aimIp;
	int localPort;
private slots:
	void on_pushButton_clicked();
	void on_pushButton_2_clicked();
};
