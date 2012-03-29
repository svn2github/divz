#include "MainWindow.h"
#include "AppSettings.h"

#include <QApplication>
#include <QtGui>

#include <QtTest/QtTest>

#include "TestClass.h"
#include "bible/BibleTestClass.h"

#define ExecTest(arg) arg object##arg; QTest::qExec(&object##arg, argc, argv);

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	AppSettings::initApp("DVizControl");

	AppSettings::load();

	// Many core routines in DViz *REQUIRE* MainWindow::mw() to exist and crash if it doesnt
	(void*)new MainWindow();
	
	ExecTest(TestMainWindow);
	ExecTest(BibleTestClass);
	
	
	return 0;
}
