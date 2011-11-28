#include "MidiInputAdapter.h"
#include "MidiReceiver.h"

	
/*
class DebugDummyAction : public MidiInputAction
{
	QString id() { return "e99f7bb5-271f-4203-bd87-6b2c609279de"; }
	QString name() { return "Rating"; }
	bool isFader() { return true; }
	void trigger(int value)
	{
		qDebug() << qPrintable(name())<<": "<<value;
	}
};

class DebugDummyAction2 : public MidiInputAction
{
	QString id() { return "e99f7bb5-271f-4203-bd87-6b2c609279de1234"; }
	QString name() { return "Foobar"; }
	bool isFader() { return true; }
	void trigger(int value)
	{
		qDebug() << qPrintable(name())<<": "<<value;
	}
};
*/
	
MidiInputAdapter::MidiInputAdapter()
	: QObject()
	, m_receiver(new MidiReceiver())
	, m_configMode(false)
	, m_host("")
	, m_port(3729)
{
	setupActionList();
	loadSettings();
	connect(m_receiver, SIGNAL(midiFrameReady(int,int,int)), this, SLOT(midiFrameReady(int,int,int)));
	connect(m_receiver, SIGNAL(connectionStatusChanged(bool)), this, SIGNAL(connectionStatusChanged(bool)));
}

MidiInputAdapter::~MidiInputAdapter()
{
	if(m_receiver)
	{
		m_receiver->exit();
		m_receiver->deleteLater();
		m_receiver = 0;
	}
}

bool MidiInputAdapter::triggerMappingForKey(int key, int value)
{
	MidiInputAction *action = actionForKey(key);
	if(action)
	{
		if(action->isFader())
			action->trigger(value);
		else
			if(value == 0) // button release
				action->trigger(value);
		return true;
	}
	else
	{
		qDebug() << "MidiInputAdapter::triggerMappingForKey: Key "<<key<<" not mapped";
		return false; 
	}
}

int MidiInputAdapter::keyForAction(MidiInputAction *action)
{
	QList<int> keys = m_mappings.keys(action);
	return keys.isEmpty() ? -1 : keys.first();
}

MidiInputAction *MidiInputAdapter::actionForKey(int key)
{
	if(!m_mappings.contains(key))
		return 0;
		
	return m_mappings.value(key);
}

bool MidiInputAdapter::isConnected()
{
	return m_receiver->isConnected();
}

void MidiInputAdapter::setMappings(QHash<int,MidiInputAction*> hash)
{
	m_mappings = hash;
	if(!m_configMode)
		storeSettings();
}

void MidiInputAdapter::setHost(QString host, int port)
{
	m_host = host;
	m_port = port;
	m_receiver->start(host, port);
	if(!m_configMode)
		storeSettings();	
}

void MidiInputAdapter::setConfigMode(bool flag)
{
	m_configMode = flag;
	if(!flag)
		storeSettings();
}

void MidiInputAdapter::midiFrameReady(int chan, int key, int val)
{
	Q_UNUSED(chan);
	
	if(!m_configMode)
		triggerMappingForKey(key, val);
	
	emit midiKeyReceived(key, val);
}

void MidiInputAdapter::setupActionList()
{
	/// REIMPL in subclasses
	m_actionList << new MidiInputAction(); // hello, world basic implementation
	//m_actionList << new DebugDummyAction() << new DebugDummyAction2();
}


void MidiInputAdapter::storeSettings()
{
	QSettings settings;
	settings.setValue("MidiInputAdapter/host", host());
	settings.setValue("MidiInputAdapter/port", port());
	
	QHash<QString,QVariant> mappings;
	foreach(int key, m_mappings.keys())
	{
		MidiInputAction *act = m_mappings[key];
		if(act)
			mappings[QString("%1").arg(key)] = QVariant(act->id());
	}
	
	settings.setValue("MidiInputAdapter/mappings", mappings);
}

void MidiInputAdapter::loadSettings()
{
	QSettings settings;
	m_host = settings.value("MidiInputAdapter/host", "").toString();
	m_port = settings.value("MidiInputAdapter/port", 3729).toInt();
	
	QMap<QString,MidiInputAction*> id2act;
	foreach(MidiInputAction *act, m_actionList)
		id2act[act->id()] = act;
	
	QHash<QString,QVariant> mappings = settings.value("MidiInputAdapter/mappings").toHash();
	QHash<int,MidiInputAction*> newMappings;
	foreach(QString key, mappings.keys())
		newMappings[key.toInt()] = id2act[mappings[key].toString()];
	
	m_mappings = newMappings;
	
	m_receiver->start(m_host, m_port);
}

