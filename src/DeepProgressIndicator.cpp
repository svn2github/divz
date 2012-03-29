#include "DeepProgressIndicator.h"
#include <QDebug>
#include <QApplication>

QHash<void*,DeepProgressIndicator*> DeepProgressIndicator::static_indicatorMap;

DeepProgressIndicator::DeepProgressIndicator(void * worker, QWidget * parent)
	: QObject(parent)
	, m_size(0)
	, m_value(0)
	, m_text("")
	, m_title("")
	, m_dialog(0)
	, m_worker(worker)
{
	static_indicatorMap[worker] = this;
	
	m_dialog = new QProgressDialog(parent);
	m_dialog->setMinimumDuration(500);
	m_dialog->setAutoClose(false);
	
}

DeepProgressIndicator::~DeepProgressIndicator()
{
	static_indicatorMap[m_worker] = 0;
	if(m_dialog)
	{
		m_dialog->close();
		delete m_dialog;
		m_dialog = 0;
	}
}
	
DeepProgressIndicator * DeepProgressIndicator::indicatorForObject(void *worker)
{
	return static_indicatorMap[worker];
}
	
void DeepProgressIndicator::step()
{
	m_value = m_value + 1;
	if(m_dialog)
		m_dialog->setValue(m_value);
	QApplication::processEvents();
	//qDebug() << "DeepProgressIndicator::step(): "<<m_text<<":"<<m_value<<"/"<<m_size;
}
	
void DeepProgressIndicator::setSize(int size)
{
	m_size = size;
	if(m_dialog)
		m_dialog->setMaximum(size);
}
	
void DeepProgressIndicator::setValue(int value)
{
	if(m_dialog)
		m_dialog->setValue(value);
	m_value = value;
}

void DeepProgressIndicator::setText(const QString &text)
{
	m_text = text;
	if(m_dialog)
		m_dialog->setLabelText(text);
}

void DeepProgressIndicator::setTitle(const QString &title)
{
	m_title = title;
	if(m_dialog)
		m_dialog->setWindowTitle(title);
}

void DeepProgressIndicator::show()
{
	if(m_dialog)
		m_dialog->show();
}

void DeepProgressIndicator::close()
{
	if(m_dialog)
		m_dialog->close();
	deleteLater();
}
