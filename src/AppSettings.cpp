#include "AppSettings.h"

#include <QSettings>

#include <QList>
#include "model/Output.h"

QList<Output*> AppSettings::m_outputs;
bool AppSettings::m_useOpenGL = false;

double AppSettings::m_liveAspect = 0;
// double AppSettings::m_docAspect = 0;

void AppSettings::load()
{
	m_outputs.clear();

	QSettings s;
	loadOutputs(&s);
	
	m_useOpenGL = s.value("app/use-opengl").toBool();
	
	updateLiveAspectRatio();
}

void AppSettings::save()
{
	QSettings s;
	
	s.setValue("app/use-opengl",m_useOpenGL);
	
	saveOutputs(&s);
	
	updateLiveAspectRatio();
}


void AppSettings::saveOutputs(QSettings *s)
{
	s->setValue("outputs/num",m_outputs.size());
	int num = 0;
	foreach(Output *out, m_outputs)
	{
		s->setValue(QString("outputs/output%1").arg(num++), out->toByteArray());
	}

}

void AppSettings::loadOutputs(QSettings *s)
{
	int num = s->value("outputs/num").toInt();
	for(int x=0; x<num;x++)
	{
		Output * out = new Output();
		out->fromByteArray(s->value(QString("outputs/output%1").arg(x)).toByteArray());
		m_outputs.append(out);
	}

	if(num == 0)
	{
		setupSystemPresetOutputs();
	}

	
}

void AppSettings::setupSystemPresetOutputs()
{
	Output * out = new Output();
		out->setIsSystem(true);
		out->setIsEnabled(true);
		out->setName("Live");
		out->setOutputType(Output::Screen);
		out->setScreenNum(0);
		out->setTags("live");
	m_outputs << out;

	out = new Output();
		out->setIsSystem(true);
		out->setIsEnabled(false);
		out->setName("Foldback");
		out->setOutputType(Output::Screen);
		out->setScreenNum(1);
		out->setTags("foldback");
	m_outputs << out;

	out = new Output();
		out->setIsSystem(true);
		out->setIsEnabled(false);
		out->setName("Secondary");
		out->setOutputType(Output::Screen);
		out->setScreenNum(2);
		out->setTags("secondary");
	m_outputs << out;
}

void AppSettings::addOutput(Output *out) { m_outputs.append(out); }

bool AppSettings::removeOutput(Output *out)
{
	if(out->isSystem())
		return false;

	m_outputs.removeAll(out);
	return true;
}

Output * AppSettings::taggedOutput(QString tag)
{
	foreach(Output *out, m_outputs)
		if(out->tags().indexOf(tag) > -1 || out->name().toLower().indexOf(tag) > -1)
			return out;
	return 0;
}

void AppSettings::setUseOpenGL(bool f)
{
	m_useOpenGL = f;
}


void AppSettings::updateLiveAspectRatio()
{
	Output *out = taggedOutput("live");
	if(out)
	{
		m_liveAspect = out->aspectRatio();
	}
	else
	{
		m_liveAspect = -1;
	}
}

