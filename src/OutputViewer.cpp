#include "OutputViewer.h"

#include "MyGraphicsScene.h"
#include "model/SlideGroup.h"
#include "model/Slide.h"
#include "model/Output.h"
#include "OutputInstance.h"

#include <QVBoxLayout>


OutputViewer::OutputViewer(OutputInstance *inst, QWidget *parent)
	    : QWidget(parent), m_inst(0)
{
	m_view = new OutputInstance(Output::previewInstance());
	m_view->setCanZoom(true);
	m_view->setFadeSpeed(0);
	m_view->setFadeQuality(0);
	
	if(inst)
		setOutputInstance(inst);
	
	m_view->setBackground(Qt::gray);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(m_view);
	setLayout(layout);
}

OutputViewer::~OutputViewer()
{
	if(m_view)
	{
		delete m_view;
		m_view = 0;
	}
}

void OutputViewer::setOutputInstance(OutputInstance *inst)
{
//  	if(m_inst)
//  		disconnect(m_inst,0,this,0);
	if(m_inst)
		m_inst->removeMirror(m_view);
 	m_inst = inst;
 	m_inst->addMirror(m_view);
 	//connect(m_inst, SIGNAL(slideChanged(int)), m_view, SLOT(setSlide(int)));
 	//connect(m_inst, SIGNAL(slideGroupChanged(SlideGroup*,Slide*)), m_view, SLOT(setSlideGroup(SlideGroup*,Slide*)));
}
