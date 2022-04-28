#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QMessageBox>
#include "powerkey_global.h"
#include "powerkey.h"
#include <direct.h>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
	//powerKey pk;
	//int b = pk.getUAVIndex(".\\key.dat");
	////int b = 0;
	//if (b != 1)
	//{
	//	QMessageBox::information(NULL, u8"´íÎó", u8"Î´ÊÚÈ¨!");
	//	return 0;
	//}

    MainWindow w;
    w.show();
    return a.exec();
}
