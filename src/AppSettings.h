#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>
#include <QList>
#include "model/Output.h"


class AppSettings
{
public:
	static void load();
	static void save();

	static QList<Output*> outputs() { return m_outputs; }
	static int numOutputs() { return m_outputs.size(); }
	static void addOutput(Output*);
	static bool removeOutput(Output*);

	static bool useOpenGL() { return m_useOpenGL; }
	static void setUseOpenGL(bool);

private:
	static void setupSystemPresetOutputs();
	static void loadOutputs(QSettings*);
	static void saveOutputs(QSettings*);
	static QList<Output*> m_outputs;
	static bool m_useOpenGL;
};

#endif
