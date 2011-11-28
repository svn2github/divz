#ifndef DVizMidiInputAdapter_H
#define DVizMidiInputAdapter_H

#include "MidiInputAdapter.h"

class DVizMidiInputAdapter : public MidiInputAdapter
{
public:
	static DVizMidiInputAdapter *instance();
	
protected:
	DVizMidiInputAdapter();
	
	virtual void setupActionList();

private:
	static DVizMidiInputAdapter *m_inst;
};

#endif
