#ifndef MidiInputAdapter_H
#define MidiInputAdapter_H

#include <QtGui>
#include <QDebug>

class MidiReceiver;

class MidiInputAction : public QObject
{
	//Q_OBJECT
public:
	MidiInputAction() {} //QObject *parent=0) : QObject(parent) {};
	virtual ~MidiInputAction() {} 
	
	// Just return a UUID - makes life easier. Used for mapping persistance across application executions
	virtual QString id() { return "0000-0000-0000-0000"; }
	
	// Displayed to user in config dialog
	virtual QString name() { return "Hello, World"; }
	
	// Fader or Button
	virtual bool isFader() { return false; }

	// Reimplement this to do the real work
	virtual void trigger(int value) { qDebug() << "Hello, World! Value is:"<<value; }
};

class MidiInputAdapter : public QObject
{
	Q_OBJECT
public:	
	MidiInputAdapter();
	~MidiInputAdapter();

	QList<MidiInputAction*> availableActions() { return m_actionList; }
	
	bool triggerMappingForKey(int key, int value);
	bool isConfigMode() { return m_configMode; }
	
	QString host() { return m_host; }
	int port() { return m_port; }

	QHash<int, MidiInputAction*> mappings() { return m_mappings; }
	
	int keyForAction(MidiInputAction*);
	MidiInputAction *actionForKey(int key);
	
	bool isConnected();

public slots:
	void setMappings(QHash<int,MidiInputAction*>);
	void setHost(QString host, int port=3729);
	
	// if true, disables mapping executor
	void setConfigMode(bool);
	
signals:
	// used mainly to configure and update mappings
	void midiKeyReceived(int key, int val);
	void connectionStatusChanged(bool);

protected slots:
	void midiFrameReady(int chan, int key, int val);


protected:
	virtual void setupActionList();
	
	void storeSettings();
	void loadSettings();

protected:
	QHash<int,MidiInputAction*> m_mappings;
	QList<MidiInputAction*> m_actionList;
	MidiReceiver *m_receiver;
	bool m_configMode;
	
	QString m_host;
	int m_port;

};

#endif
