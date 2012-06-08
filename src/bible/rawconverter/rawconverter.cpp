#include <QCoreApplication>
#include <QtGui>

int main(int argc, char **argv)
{
	if(argc < 4)
	{
		printf("Usage: %s <input book file> <input texts file, ex> <output data file> <output code> <output name>\n", argv[0]);
		printf("Example: %s BIBLE_BOOKS_ENGLISHNIV ENGLISHNIV_BIBLE englishniv.dvizbible NIV 'New International Version'\n\n", argv[0]);
		exit(-1);
	}
	
	QString inputBooks = argv[1];
	QString inputTexts = argv[2];
	QString outputData = argv[3];
	QString outputCode = argv[4];
	QString outputName = argv[5];
	
	qDebug() << "rawconverter started, reading book index from "<<inputBooks<<", Bible text from "<<inputTexts<<", outputting data structure to: "<<outputData;
	
	
	// NOTE Info on data structure:
	// Data output is a QVariantMap
	//	- outputName (displayed in ComboBox in BibleBrowser in DViz)
	//	- outputCode (used to index this version and to locate this version in BibleConnector.cpp)
	//	- bookNames (QVariantMap, key is the english book name, value is QVariantMap with following keys: EnglishName, DisplayName, ChapterCount)
	//	- bibleText
	//		- QVariantMap, key is English Book Name, value is QVariantList
	//			- index is chapter number -1
	//			- list contains QList of verses
	//				- index is verse number -1
	//				- list contains QString of verse text
	// Note: DisplayName is stored in the bookNames map because it may be a localized string for the name of the book
	// 	For example, the entry for Genesis would have EnglishName as 'Genesis', and DisplayName as 'ஆதியாகமம்'
	
	QVariantMap bookNames;
	QVariantMap bibleText;
	
	QHash<QString,QString> bookNumToName;
	
	/// First, read in inputBooks and build bookNames map
	{
		QFile file(inputBooks);
		if(!file.open(QIODevice::ReadOnly))
		{
			qDebug() << "Cannot read book input file: "<<inputBooks;
			exit(-1);
		}
	
		QTextStream stream(&file);
		QString fileContents = stream.readAll();
		file.close();
		
		QStringList lines = fileContents.split("\n");
		
		int lineCounter = 1;
		
		// Example line: 1,Genesis,47656e65736973,50
		foreach(QString line, lines)
		{
			QStringList columns = line.split(",");
			if(columns.size() < 4)
			{
				if(!line.isEmpty())
					qDebug() << "Error at " << qPrintable(QString("%1:%2").arg(inputBooks).arg(lineCounter))<<": Invalid number of columns: "<<columns.size()<<", string: "<<line;
				continue;
			}
			
			int bookNum         = columns[0].toInt();
			QString englishBook = columns[1];
			QString hexBook     = columns[2];
			int chapCount       = columns[3].toInt();
			
			// Convert the hext string in col 2 to a UTF-8 string
			QString localBook = QString::fromUtf8(QByteArray::fromHex(QByteArray().append(hexBook)).data()).trimmed();
			
			QVariantMap bookData;
			bookData["EnglishName"]  = englishBook;
			bookData["DisplayName"]  = localBook;
			bookData["ChapterCount"] = chapCount;
			
			// Include book data under both the english and local strings for easier indexing
			bookNames[englishBook] = bookData;
			bookNames[localBook]   = bookData;
			
			// Book Key in the text data is suffixed with a letter 'O' if <40, or the letter 'N' if >=40 - don't ask me why, I don't know
			QString bookNumKey = QString("%1%2").arg(bookNum).arg(bookNum < 40 ? 'O' : 'N');
			bookNumToName[bookNumKey] = englishBook;
			
			// Some texts use a 0 before the number if the num <10
			if(bookNum < 10)
			{
				QString altBookNumKey = QString("0%1%2").arg(bookNum).arg(bookNum < 40 ? 'O' : 'N');
				bookNumToName[altBookNumKey] = englishBook;
				//qDebug() << "bookNumToName: "<<altBookNumKey<<" ("<<bookNum<<") -> "<<englishBook;
			}
				
			//qDebug() << "bookNumToName: "<<bookNumKey<<" ("<<bookNum<<") -> "<<englishBook;
			
			lineCounter ++;
		}
	}
	
	/// Next, read in inputTexts and build bibleText map
	{
		//   book           chap       verse verseText
		QHash<QString, QHash<int, QHash<int, QString > > > bibleTextHash;
		
		QFile file(inputTexts);
		if(!file.open(QIODevice::ReadOnly))
		{
			qDebug() << "Cannot text book input file: "<<inputTexts;
			exit(-1);
		}
	
		QTextStream stream(&file);
		QString fileContents = stream.readAll();
		file.close();
		
		QStringList lines = fileContents.split("\n");
		
		int lineCounter = 1;
		
		// Line schema:
		// - VerseID  (int)
		// - Book     (varchar(3))
		// - Chapter  (int)
		// - Verse    (int)
		// - Data     (blob)
		// Example line: 1,1O,1,1,20496e2074686520626567696e6e696e6720476f642063726561746564207468652068656176656e7320616e64207468652065617274682e
		foreach(QString line, lines)
		{
			QStringList columns = line.split(",");
			if(columns.size() < 5)
			{
				if(!line.isEmpty())
					qDebug() << "Error at " << qPrintable(QString("%1:%2").arg(inputBooks).arg(lineCounter))<<": Invalid number of columns: "<<columns.size()<<", string: "<<line;
				continue;
			}
			
			QString bookNumKey = columns[1];
			int chapter        = columns[2].toInt();
			int verse          = columns[3].toInt();
			QString hexBlob    = columns[4];
		
			// Convert the hext string in col 2 to a UTF-8 string
			QByteArray utfBytes = QByteArray::fromHex(QByteArray().append(hexBlob)).data();
			QString verseText = QString::fromUtf8(utfBytes).trimmed();
			
			QString bookName = bookNumToName[bookNumKey];
			
			if(!bibleTextHash.contains(bookName))
				bibleTextHash[bookName] = QHash<int, QHash<int, QString > >();
			
			if(!bibleTextHash[bookName].contains(chapter))
				bibleTextHash[bookName][chapter] = QHash<int, QString >();
			
			bibleTextHash[bookName][chapter][verse] = verseText;
			
			lineCounter ++;
			
			/*
			qDebug() << "line: "<<line;
			qDebug() << "cols: "<<columns;
			qDebug() << "bookNumKey: "<<bookNumKey;
			qDebug() << "bookName:   "<<bookName;
			qDebug() << "chapter:    "<<chapter;
			qDebug() << "verse:      "<<verse;
			qDebug() << "hex:        "<<hexBlob;
			qDebug() << "text:       "<<verseText;
			printf(     "raw text:     %s\n", utfBytes.data());
			printf(     "raw text 2:   %s\n", verseText.toUtf8().data());
			
			QVariant var = verseText;
			QVariantMap map1;
			map1["test"] = var;
			
			printf(     "var2raw:      %s\n", map1["test"].toString().toUtf8().data());
			*/
			/*
			qDebug() << "test done";
			exit(-1);
			*/
			//break;
			
		}
		
		// Convert the bibleTextHash (using templates) to a QVariantMap for storage
		foreach(QString book, bibleTextHash.keys())
		{
			QVariantMap bookMap;
			foreach(int chap, bibleTextHash[book].keys())
			{
				QVariantMap chapMap;
				foreach(int verse, bibleTextHash[book][chap].keys())
				{
					chapMap[QString::number(verse)] = bibleTextHash[book][chap][verse]; 
				}
				bookMap[QString::number(chap)] = chapMap;
			}
			bibleText[book] = bookMap;
		}
		
		//qDebug() << "Test: "<<bibleText["Genesis"].toMap()["1"].toMap()["1"];
		//qDebug() << "Test: "<<bibleTextHash["Genesis"][1][1];
		//printf("raw test:     %s\n", bibleTextHash["Revelation"][1][1].toUtf8().data());
	}
	
	/// Finally, build dataOutput map and write to disk
	
	QVariantMap dataOutput;
	dataOutput["name"] = outputName;
	dataOutput["code"] = outputCode;
	dataOutput["bookNames"] = bookNames;
	dataOutput["bibleText"] = bibleText;
	
	// write to disk
	QFile file(outputData);
	if(!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "Error opening "<<outputData<<" for writing, exiting.";
		exit(-1);
	}
	
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	
	stream << dataOutput;
	
	file.write(array);
	file.close();
	
	qDebug() << "rawconverter finished, wrote "<<array.size()<<" bytes to "<<outputData;
	
}
