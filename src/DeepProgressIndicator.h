#ifndef DeepProgressIndicator_H
#define DeepProgressIndicator_H

#include <QWidget>
#include <QObject>
#include <QProgressDialog>
#include <QHash>

class DeepProgressIndicator : public QObject
{
	Q_OBJECT
public:
	DeepProgressIndicator(void * worker, QWidget * parent = 0);
	~DeepProgressIndicator();
	
	static DeepProgressIndicator * indicatorForObject(void*);
	
	void step();
	
	void setSize(int);
	int size() { return m_size; }
	
	void setValue(int);
	int value() { return m_value; }
	
	void setText(const QString &);
	QString text() { return m_text; }
	
	void setTitle(const QString &);
	QString title() { return m_title; }
	
	QProgressDialog * dialog() { return m_dialog; }
	
	void show();
	void close();

private:
	int m_size;
	int m_value;
	QString m_text;
	QString m_title;
	QProgressDialog * m_dialog;
	void * m_worker;
	
	static QHash<void*,DeepProgressIndicator*> static_indicatorMap;
};

#endif
