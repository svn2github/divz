#include "TestClass.h"

#include <QTest>

#include "MainWindow.h"

void TestMainWindow::mwPointer()
{
	QVERIFY(MainWindow::mw() != NULL);
}
