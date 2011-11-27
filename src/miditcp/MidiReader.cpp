#include "MidiReader.h"

#include <QDebug>

#include <sys/soundcard.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


MidiReader::MidiReader(const QString& device, int chan, QObject *parent)
	: QThread(parent)
	, m_device(device)
	, m_midiChan(chan)
	, m_lastPak(0)
	, m_readCount(0)
	, m_killed(false)
{
	
}

MidiReader::~MidiReader()
{
	m_killed = true;
	wait();
}


void MidiReader::run()
{
	m_file.setFileName(m_device);
	if(!m_file.open(QIODevice::ReadOnly))
	{
		qDebug() << "MidiReader: Unable to open device "<<m_device;
	}
	
	while(!m_killed)
	{
		int bytes = m_file.read((char*)&m_inpacket, sizeof(m_inpacket));
		
		if(m_inpacket[0] == SEQ_MIDIPUTC)
		{
			unsigned char val = m_inpacket[1]; 
			if(val != m_lastPak)
			{
				if(m_readCount == 0)
				{
					if(val != (unsigned char)m_midiChan)
					{
						m_lastPak = val;
						continue;	
					}
				}
				
				m_finalPak[m_readCount] = val;
				
				//printf("MidiReader: [DEBUG] Read: %d, readCount: %d (last: %d)\n", m_inpacket[1], m_readCount, m_lastPak);
				m_readCount ++;
				
				if(m_readCount == 3)
				{
					int a = (int)m_finalPak[0],
					b = (int)m_finalPak[1],
					c = (int)m_finalPak[2];
					
					//printf("Read MIDI packet: %d %d %d\n",a,b,c);
						
					emit midiFrameReady(a,b,c);
						
					m_readCount = 0;
					//msleep(33);
				}
				
				m_lastPak = val;
			}
		}
		
		if(bytes <= 0)
		{
			//qDebug() << "33ms sleep";
			msleep(33);
		}
		
	
	}
	
	m_file.close();
	
}
