#include "RtfEditorWindow.h"
#include <QtGui>
#include "../livemix/EditorUtilityWidgets.h"
#include "../3rdparty/richtextedit/richtexteditor_p.h"
#include "GLTextDrawable.h"

RtfEditorWindow::RtfEditorWindow(GLTextDrawable *gld, QWidget *parent)
	: QDialog(parent)
	, m_gld(gld)
{
	QVBoxLayout *layout = new QVBoxLayout();
		
	PropertyEditorFactory::PropertyEditorOptions opts;

	opts.reset();
	
	QHBoxLayout *countdown = new QHBoxLayout();
	QWidget *boolEdit = PropertyEditorFactory::generatePropertyEditor(gld, "isCountdown", SLOT(setIsCountdown(bool)), opts);
	QWidget *dateEdit  = PropertyEditorFactory::generatePropertyEditor(gld, "targetDateTime", SLOT(setTargetDateTime(QDateTime)), opts);
	QCheckBox *box = dynamic_cast<QCheckBox*>(boolEdit);
	if(box)
	{
		connect(boolEdit, SIGNAL(toggled(bool)), dateEdit, SLOT(setEnabled(bool)));
		dateEdit->setEnabled(box->isChecked());
	}
	countdown->addWidget(boolEdit);
	countdown->addWidget(dateEdit);
	countdown->addStretch(1);
	layout->addLayout(countdown);
	
	
	m_rtfEditor = new RichTextEditorDialog();
	m_rtfEditor->setText(gld->text());
	//m_editor->initFontSize(m_model->findFontSize());
	
	//m_rtfEditor->show();
	layout->addWidget(m_rtfEditor);
	//m_rtfEditor->adjustSize();
	
	QPushButton *okButton = new QPushButton(tr("&Save"));

	okButton->setDefault(true);
	connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
	
	QPushButton *closeButton = new QPushButton(tr("Cancel"));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
	
	setWindowTitle("Edit Text");
	
	QBoxLayout *controlLayout = new QHBoxLayout;
	controlLayout->addStretch(1);
	controlLayout->addWidget(closeButton);
	controlLayout->addWidget(okButton);
	layout->addLayout(controlLayout);
	
	m_rtfEditor->focusEditor();
	
	setLayout(layout);
	
	readSettings();
}

void RtfEditorWindow::okClicked()
{
	m_gld->setText(m_rtfEditor->text(Qt::RichText));
	close();
}

void RtfEditorWindow::closeEvent(QCloseEvent */*event*/)
{
	writeSettings();
}

void RtfEditorWindow::readSettings()
{
	QSettings settings;
	QPoint pos = settings.value("RtfEditorWindow/pos", QPoint(10, 10)).toPoint();
	QSize size = settings.value("RtfEditorWindow/size", QSize(640,480)).toSize();
	move(pos);
	resize(size);
}

void RtfEditorWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("RtfEditorWindow/pos", pos());
	settings.setValue("RtfEditorWindow/size", size());
}

