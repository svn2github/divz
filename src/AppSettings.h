#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>
#include <QList>
#include <QMap>
#include <QSizeF>
#include <QPair>
#include <QDir>

#ifndef QStringPair
	typedef QPair<QString,QString> QStringPair;
#endif

class MainWindow;
class Output;
class QWebView;
class AppSettings
{
public:
	static void initApp(const QString& appName="DViz");
	static void load();
	static void save();
	
	typedef enum LiveEditMode
	{
		LiveEdit,
		SmoothEdit,
		PublishEdit
	};
	
	static LiveEditMode liveEditMode() { return m_liveEditMode; }
	static void setLiveEditMode(LiveEditMode);

	static QList<Output*> outputs() { return m_outputs; }
	static int numOutputs() { return m_outputs.size(); }
	static void addOutput(Output*);
	static bool removeOutput(Output*);
	static Output * taggedOutput(QString tag);
	static Output * outputById(int id);

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
	
	static int pixmapCacheSize() { return m_pixmapCacheSize; }
	static void setPixmapCacheSize(int);
	
	static int crossFadeSpeed() { return m_crossFadeSpeed; }
	static void setCrossFadeSpeed(int);
	
	static int crossFadeQuality() { return m_crossFadeQuality; }
	static void setCrossFadeQuality(int);
	
	static int autosaveTime() { return m_autosaveTime; }
	static void setAutosaveTime(int);
	
	static QString cachePath() { return cacheDir().absolutePath(); }
	static QDir cacheDir() { return m_cacheDir; }
	static void setCacheDir(const QDir &);
	
	typedef QList<QStringPair> ResourcePathTranslations;
	
	static void setResourcePathTranslations(ResourcePathTranslations);
	static ResourcePathTranslations resourcePathTranslations() { return m_resourcePathTranslations; }
	static QString applyResourcePathTranslations(const QString&);
	static QDir    applyResourcePathTranslations(const QDir&);
	
	static bool httpControlEnabled() { return m_httpControlEnabled; }
	static void setHttpControlEnabled(bool);
	
	static int httpControlPort() { return m_httpControlPort; }
	static void setHttpControlPort(int);
	
	static bool httpViewerEnabled() { return m_httpViewerEnabled; }
	static void setHttpViewerEnabled(bool);
	
	static int httpViewerPort() { return m_httpViewerPort; }
	static void setHttpViewerPort(int);
	
	static bool httpTabletServerEnabled() { return m_httpTabletServerEnabled; }
	static void setHttpTabletServerEnabled(bool);
	
	static int httpTabletServerPort() { return m_httpTabletServerPort; }
	static void setHttpTabletServerPort(int);
	
	static QString myIpAddress();
	
	static void setHotkeySequence(const QString& actionName, const QString& keySequence);
	static QString hotkeySequence(const QString& actionName);
	
	static void setTemplateStorageFolder(const QString& dir);
	static QString templateStorageFolder() { return m_templateStorageFolder; }
	
	static double titlesafeAmount() { return m_titlesafeAmount; }
	static void setTitlesafeAmount(double);
	static QRect adjustToTitlesafe(QRect);
	static QRectF adjustToTitlesafe(QRectF);
	
	static bool isStatSendingEnabled() { return m_isStatSendingEnabled; }
	static void setStatSendingEnabled(bool);
	
	static QString registrationName()  { return m_regName; }
	static void setRegistrationName(QString);
	
	static QString registrationOrgName() {  return m_regOrgName; }
	static void setRegistrationOrgName(QString);
	
	static void sendCheckin(QString path, QString data="");
	
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

	static int m_pixmapCacheSize;
	static int m_crossFadeSpeed; // ms
	static int m_crossFadeQuality; // frames
	
	static LiveEditMode m_liveEditMode;
	
	static int m_autosaveTime; //seconds
	
	static QDir m_cacheDir;
	static ResourcePathTranslations m_resourcePathTranslations;
	
	static bool m_httpControlEnabled;
	static int m_httpControlPort;
	
	static bool m_httpViewerEnabled;
	static int m_httpViewerPort;
	
	static bool m_httpTabletServerEnabled;
	static int m_httpTabletServerPort;
	
	static QHash<QString,QString> m_hotkeys;
	
	static QString m_templateStorageFolder;
	
	static double m_titlesafeAmount;
	
	static bool m_isStatSendingEnabled;
	static QString m_regName;
	static QString m_regOrgName;
	
	static QWebView *m_checkinWebview;

};

#endif
