#include "MidiReceiver.h"

#include "MidiInputAdapter.h"

MidiInputAdpater::MidiInputAdapter()
	: QObject()
	, m_receiver(new MidiReceiver())
	, m_configMode(false)
	, m_host("")
	, m_port(3729)
{
	setupActionList();
	loadSettings();
	connect(m_receiver, SIGNAL(midiFrameReady(int,int,int)), this, SLOT(midiFrameReady(int,int,int)));
}

MidiInputAdpater::~MidiInputAdpater()
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
		action->trigger(value);
		return true;
	}
	else
	{
		qDebug() << "MidiInputAdapter::triggerMappingForKey: Key "<<key<<" not mapped";
		return false; 
	}
}

int MidiInputAdpater::keyForAction(MidiInputAction *action)
{
	QList<int> keys = m_mappings.keys(action);
	return keys.isEmpty() ? -1 : keys.first();
}

MidiInputAction *MidiInputAdpater::actionForKey(int key)
{
	if(!m_mappings.contains(key))
		return 0;
		
	return m_mappings.value(key);
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
	if(!m_configMode)
		triggerMappingForKey(key, val);
	
	emit midiKeyReceived(key, val);
}

void MidiInputAdapter::setupActionList()
{
	/// REIMPL in subclasses
	m_actionList << new MidiInputAction(); // hello, world basic implementation
}


void MidiInputAdapter::storeSettings()
{
	QSettings settings;
	settings.setValue("MidiInputAdapter/host", host());
	settings.setValue("MidiInputAdapter/port", port());
	
	QMap<int,QString> mappings;
	foreach(int key, m_mappings.keys())
		mappings[key] = m_mappings[key]->id();
	
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
	
	QMap<int,QString> mappings = settings.setValue("MidiInputAdapter/mappings").toMap();
	QHash<int,MidiInputAction*> newMappings;
	foreach(int key, mappings.keys())
		newMappings[key] = id2act[mappings[key]];
	
	m_mappings = newMappings;
	
	m_receiver->start(host, port);
}

