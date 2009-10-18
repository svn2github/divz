#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>
#include <QList>
#include <QMap>
#include "model/Output.h"

class MainWindow;
class AppSettings
{
public:
	static void load();
	static void save();

	static QList<Output*> outputs() { return m_outputs; }
	static int numOutputs() { return m_outputs.size(); }
	static void addOutput(Output*);
	static bool removeOutput(Output*);
	static Output * taggedOutput(QString tag);

	static bool useOpenGL() { return m_useOpenGL; }
	static void setUseOpenGL(bool);

	static QSizeF gridSize() { return m_gridSize; }
	static void setGridSize(QSizeF);

	static bool gridEnabled() { return m_gridEnabled; }
	static void setGridEnabled(bool);

	static bool thirdGuideEnabled() { return m_thirdGuideEnabled; }
	static void setThirdGuideEnabled(bool);

	static QPointF snapToGrid(QPointF, bool halfGird = false);


	static double liveAspectRatio() { return m_liveAspect; }
// 	static double currentDocumentAspectRatio() { return m_docAspect; }
// 	static QRect standardSceneRect(double aspectRatio = -1);

	static QString previousPath(const QString& key);
	static void setPreviousPath(const QString& key, const QString & path);

protected:
	friend class MainWindow;
// 	static double setCurrentDocumentAspectRatio(double);
	static void updateLiveAspectRatio();

private:
	static void setupSystemPresetOutputs();
	static void loadOutputs(QSettings*);
	static void saveOutputs(QSettings*);

	static QList<Output*> m_outputs;
	static bool m_useOpenGL;

	static QSizeF m_gridSize;
	static bool m_gridEnabled;
	static bool m_thirdGuideEnabled;

	static double m_liveAspect;
	
	static QMap<QString,QString> m_previousPathList;
// 	static double m_docAspect;
};

#endif
