#ifndef OUTPUTVIEWER_H
#define OUTPUTVIEWER_H

#include <QWidget>

class OutputInstance;

class OutputViewer : public QWidget
{
public:
	OutputViewer(OutputInstance *output=0, QWidget *parent=0);
	~OutputViewer();

public slots:
	void setOutputInstance(OutputInstance *);
	
private:
	OutputInstance * m_inst;
	OutputInstance * m_view;

};

#endif // OUTPUTVIEWER_H
