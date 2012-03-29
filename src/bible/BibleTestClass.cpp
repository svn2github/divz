#include "BibleTestClass.h"

#include <QTest>

#include "bible/BibleModel.h"

void BibleTestClass::normalizeRef_data()
{
	QTest::addColumn<QString>("string");
	QTest::addColumn<bool>("result");
	
	QTest::newRow("single verse")		<< "John 3:16"	<< true;
	QTest::newRow("bad char in vs")		<< "John 3:!6"	<< false;
	QTest::newRow("multi verse")		<< "John 3:1-6"	<< true;
	QTest::newRow("chapter")		<< "John 3"	<< true;
	QTest::newRow("book")			<< "John"	<< false;
}

void BibleTestClass::normalizeRef()
{
	QFETCH(QString, string);
	QFETCH(bool,    result);

	QCOMPARE(BibleVerseRef::normalize(string).valid(), result);
}
