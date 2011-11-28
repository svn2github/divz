
#include "DVizMidiInputAdapter.h"

#include "AppSettings.h"
#include "MainWindow.h"
#include "DocumentListModel.h"
#include "OutputInstance.h"
#include "OutputControl.h"
#include "model/Document.h"
#include "model/Output.h"
#include "model/SlideGroup.h"
#include "model/Slide.h"
#include "model/SlideGroupFactory.h"

namespace DVizMidiAction
{
	static MainWindow *mw;
	
	class BlackButton : public MidiInputAction
	{
		QString id() { return "64258ee6-f50e-4f94-ad6a-589a93ac2d40"; }
		QString name() { return "Black Output"; }
		void trigger(int)
		{
			int liveId = AppSettings::taggedOutput("live")->id();
			OutputControl * outputControl = mw->outputControl(liveId);
			outputControl->fadeBlackFrame(!outputControl->isBlackToggled());	
		}
	};
	
	class ClearButton : public MidiInputAction
	{
		QString id() { return "ad44323f-22a2-43a5-9839-1d6476b5ca41"; }
		QString name() { return "Clear Output"; }
		void trigger(int)
		{
			int liveId = AppSettings::taggedOutput("live")->id();
			OutputControl * outputControl = mw->outputControl(liveId);
			outputControl->fadeClearFrame(!outputControl->isClearToggled());	
		}
	};
	
	class LogoButton : public MidiInputAction
	{
		QString id() { return "fb7a5663-7c8f-432b-991c-901cbf3223af"; }
		QString name() { return "Logo Output"; }
		void trigger(int)
		{
			int liveId = AppSettings::taggedOutput("live")->id();
			OutputControl * outputControl = mw->outputControl(liveId);
			outputControl->fadeLogoFrame(!outputControl->isLogoToggled());	
		}
	};
	
	class NextSlideButton : public MidiInputAction
	{
		QString id() { return "e99f7bb5-271f-4203-bd87-6b2c609279de"; }
		QString name() { return "Go to Next Slide"; }
		void trigger(int)
		{
			int liveId = AppSettings::taggedOutput("live")->id();
			SlideGroupViewControl *viewControl = mw->viewControl(liveId);
			viewControl->nextSlide();
		}
	};
	
	class PrevSlideButton : public MidiInputAction
	{
		QString id() { return "b0f8f64b-96f3-4d6f-9084-e9f145b37e0b"; }
		QString name() { return "Go to Previous Slide"; }
		void trigger(int)
		{
			int liveId = AppSettings::taggedOutput("live")->id();
			SlideGroupViewControl *viewControl = mw->viewControl(liveId);
			viewControl->prevSlide();
		}
	};
	
	class FadeSpeedFader : public MidiInputAction
	{
		QString id() { return "1188e63f-4723-424c-b5fc-a1068da669c7"; }
		QString name() { return "Set Fade Speed"; }
		bool isFader() { return true; }
		void trigger(int value)
		{
			// 127 is max for my MIDI fader
			double percent = ((double)value) / 127.0;
			// setCrossFadeSpeed() expects an integer percent from 0-100 
			double speed = 100.0 * percent;
			
			int liveId = AppSettings::taggedOutput("live")->id();
			OutputControl * outputControl = mw->outputControl(liveId);
			outputControl->setCrossFadeSpeed((int)speed);	
		}
	};
};

void DVizMidiInputAdapter::setupActionList()
{
	DVizMidiAction::mw = MainWindow::mw();
	
	m_actionList.clear();
	m_actionList
		<< new DVizMidiAction::BlackButton()
		<< new DVizMidiAction::ClearButton()
		<< new DVizMidiAction::LogoButton()
		<< new DVizMidiAction::NextSlideButton()
		<< new DVizMidiAction::PrevSlideButton()
		<< new DVizMidiAction::FadeSpeedFader();
	
	qDebug() << "DVizMidiInputAdapter::setupActionList(): Added "<<m_actionList.size() <<" actions to the MIDI Action List";
}

DVizMidiInputAdapter *DVizMidiInputAdapter::m_inst = 0;

DVizMidiInputAdapter::DVizMidiInputAdapter()
	: MidiInputAdapter()
{
	setupActionList();
	loadSettings();
}

DVizMidiInputAdapter *DVizMidiInputAdapter::instance()
{
	if(!m_inst)
		m_inst = new DVizMidiInputAdapter();
	return m_inst;
}
