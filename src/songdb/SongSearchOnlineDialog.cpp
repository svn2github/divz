#include "SongSearchOnlineDialog.h"
#include "ui_SongSearchOnlineDialog.h"

#include "SongEditorHighlighter.h"
#include "SongBrowser.h"

SongSearchOnlineDialog::SongSearchOnlineDialog(SongBrowser *sb, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::SongSearchOnlineDialog)
	, m_sb(sb)
{
	ui->setupUi(this);
	setupUi();
	setupProcs();
}

SongSearchOnlineDialog::~SongSearchOnlineDialog()
{
	delete ui;
}

void SongSearchOnlineDialog::setupUi()
{
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotSaveClicked()));
	connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(slotSearchClicked()));
	connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(currentRowChanged(int)));
	
	ui->listProgressBar->setVisible(false);
	ui->previewProgressBar->setVisible(false);
	
	ui->songTitle->setVisible(false);
	ui->songTitleLabel->setVisible(false);
	
	(void*)new SongEditorHighlighter(ui->preview->document());
}

void SongSearchOnlineDialog::setupProcs()
{	
	connect(&m_searchProc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(searchProcExit(int, QProcess::ExitStatus)));
	connect(&m_downloadProc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(downloadProcExit(int, QProcess::ExitStatus)));
}

QVariant SongSearchOnlineDialog::getJson(QProcess *proc, QString context)
{
	QByteArray stdErr = proc->readAllStandardError();
	QString err = QString(stdErr);
	if(!err.isEmpty())
		qDebug() << context<< " [error]: "<<qPrintable(err);
	
	QByteArray data = proc->readAllStandardOutput();
	return getJson(data, context);
}

QVariant SongSearchOnlineDialog::getJson(QByteArray &data, QString context)
{
	bool ok = false;
	QVariant result = m_jsonParser.parse(data, &ok);
	
	qDebug() << context<< " [json]: "<<qPrintable(QString(data));
	
	if(!ok)
	{
		qDebug() << "SongSearchOnlineDialog: "<<context<<": Error parsing json: "<<m_jsonParser.errorString()<<"\nJSON: "<<qPrintable(QString(data));
	}
	
	return result;
}

void SongSearchOnlineDialog::setText(const QString& text)
{
	ui->searchBox->setText(text);
	slotSearchClicked();
}

void SongSearchOnlineDialog::slotSearchClicked()
{
	QString searchString = ui->searchBox->text();
	
	qDebug() << "[search] Searching for:"<<searchString;
	
	m_searchProc.start("songdb/googlesong.pl", QStringList() << searchString);
	
	ui->listProgressBar->setVisible(true);
	ui->listWidget->clear();
}

void SongSearchOnlineDialog::searchProcExit(int, QProcess::ExitStatus)
{
	ui->listProgressBar->setVisible(false);
	
	QVariant json = getJson(&m_searchProc, "Search");

	QVariantList results = json.toList();
	qDebug() << "[search] Search finished, got:"<<results.size()<<" results";
	
	ui->listWidget->clear();
	
	QStringList list;
	foreach(QVariant line, results)
	{
		QVariantMap data = line.toMap();
		
		QString title   = data["title"].toString();
		QString summary = data["summary"].toString();
		QString url     = data["url"].toString();
		
		qDebug() << "[search] Adding Item:"<<title<<", url:"<<url;
		
		list << tr("%1 - %2").arg(title).arg(summary);
	}
	
	ui->listWidget->addItems(list);
	
	m_resultset = results;
}

void SongSearchOnlineDialog::currentRowChanged(int row)
{
	if(row < 0 || row > m_resultset.size() - 1)
		return;
		
	QVariant line = m_resultset[row];
	QVariantMap map = line.toMap();
	QString url = map["url"].toString();
	
	qDebug() << "[cell clicked] Downloading URL:"<<url;
	
	m_downloadProc.start("songdb/getlyrics.pl", QStringList() << url);

	ui->previewProgressBar->setVisible(true);
	
	ui->songTitle->setVisible(false);
	ui->songTitleLabel->setVisible(false);
	
	ui->preview->setText("");
}

void SongSearchOnlineDialog::downloadProcExit(int code, QProcess::ExitStatus status)
{
	QString fileName = "/tmp/lyrics.json";
	
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this,tr("Can't Read File"),QString(tr("Unable to open %1")).arg(fileName));
		return;
	}

	QTextStream stream(&file);
	QString fileContents = stream.readAll();
	
	QByteArray data;
	data.append(fileContents);
	
	QVariant json = getJson(data, "Download");

	QVariantMap results = json.toMap();
	
	ui->songTitle->setVisible(true);
	ui->songTitleLabel->setVisible(true);
	
	ui->previewProgressBar->setVisible(false);
	
	QString text = results["text"].toString();
	QString title = results["title"].toString();
	
	qDebug() << "[download exit] Done downloading, got title: "<<title;
	
	ui->preview->setText(text);
	ui->songTitle->setText(title);
}

void SongSearchOnlineDialog::slotSaveClicked()
{
	QString title = ui->songTitle->text();
	QString text = ui->preview->toPlainText();
	
	qDebug() << "[save] Adding song "<<title;
	
	m_sb->addNewSong(title,text,true); // true=add to doc
	
	close();
}


void SongSearchOnlineDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
