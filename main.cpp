#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainwindow w;
	w.show();


	return a.exec();	
}

