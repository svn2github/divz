#include "EntityList.h"

bool EntityList::m_init = false;
QHash<QString,QString> EntityList::m_dec;
QHash<QString,QString> EntityList::m_enc;
	
void EntityList::init()
{
	if(m_init)
		return;
		
	char str[2];
	str[1] = 0;
	
	QString chr;
	QString name;
	QString ent;
	
	for (int i = 0; i < NUM_MXP_ENTITIES; i++)
	{
		str[0] = ENTITY_DEF[i];
		chr  = QString::fromLatin1(str);
		
		name = QString::fromLatin1(ENTITY_NAMES[i]);
		ent  = QString("&%1;").arg(name);
		
		m_dec[ent] = chr;
		m_enc[chr] = ent;
	}
	
	m_init = true;
}

QString EntityList::decodeEntities(QString str)
{
	init();
	
	foreach(QString ent, m_dec.values())
		str = str.replace(ent, m_dec[ent]);
	
	return str;
}

QString EntityList::encodeEntities(QString str)
{
	init();
	
	foreach(QString ent, m_enc.values())
		str = str.replace(ent, m_enc[ent]);
	
	return str;
}

