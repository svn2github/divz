#include "PPTLoader.h"
#include <QDebug>
#include <QFile>

#include "powerpointviewer.h"

//MSScriptControl::ScriptControl * PPTLoader::m_script = 0;

PPTLoader::PPTLoader()
	: QObject()
	, m_numSlides(-2)
{
	setupScripts();
}

PPTLoader::~PPTLoader()
{
	showExit();
	closeFile();
	closePPT();
	#ifdef WIN32_PPT_ENABLED
	delete m_script;
	#endif

}


void PPTLoaderTest::test1()
{
    PPTLoader *ppt = new PPTLoader();
    ppt->openFile("c:/test3.ppt");
    qDebug() << "Num Slides #1: "<<ppt->numSlides();
    delete ppt;
}

void PPTLoaderTest::test2()
{
    PPTLoader *ppt = new PPTLoader();
    ppt->openFile("c:/test2.ppt");
    qDebug() << "Num Slides #2: "<<ppt->numSlides();
    delete ppt;
}

void PPTLoader::axException(int code,QString source,QString description,QString help)
{
	#ifdef WIN32_PPT_ENABLED
	MSScriptControl::ScriptControl * sm = dynamic_cast<MSScriptControl::ScriptControl*>(sender());
	if(sm)
	{
		MSScriptControl::IScriptError * error = sm->Error();
		if(error->Number() == 0)
			return;
		qDebug() << "[ScriptControl Error] Error#"<<error->Number()<<": Line"<<error->Line()<<": Col"<<error->Column()<<": "<<error->Description()<<"\n\tSource:"<<error->Text();
	}
	else
	{
		qDebug() << "[AxException] Code"<<code<<": "<<source<<":"<<description<<"("<<help<<")";
	}
	#endif
}

void PPTLoader::setupScripts()
{
	//if(m_script)
	//	return;
#ifdef WIN32_PPT_ENABLED
	m_script = new MSScriptControl::ScriptControl();
	
	connect(m_script, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(axException(int,QString,QString,QString)));
	m_script->SetLanguage("JScript");
	
	
	MSScriptControl::ScriptControl * sm = m_script;
	
	//QString sampleJs = QString("var ppApp = new ActiveXObject(\"Powerpoint.Application\");"
		//			"ppApp.Visible=true;"
		//			"ppApp.WindowState=2;"
		//			"var ppPres = ppApp.Presentations.Open(\"c:/test.ppt\");"
		//			"ppPres.SlideShowSettings.Run();"
	//
	//				);
	
	// Setup Global object
	sm->ExecuteStatement("function Global() {}; Global.loaded = 1; Global.ppPres=0;Global.ppApp=0;Global.ppShowWin=0;");
	
	// openPPT()
	sm->ExecuteStatement("Global.openPPT = function() {"
				"if(Global.ppApp){ return; }"
				"var ppApp = new ActiveXObject(\"Powerpoint.Application\");"
				"ppApp.Visible=true;"
				"ppApp.WindowState=2;"
				"Global.ppApp = ppApp;"
				//"Global.ppApp.Presentations.Open('c:/test.ppt');"
				"}");
	// openFile(name)
	sm->ExecuteStatement("Global.openFile = function(file) {"
				"if(!Global.ppApp) {Global.openPPT();}"
				"if(Global.ppPres) {throw new Error('A file is already open - use Global.closeFile() first!')}"
				"Global.ppPres = Global.ppApp.Presentations.Open(file);"
				"}");
	
	// numSlides() : int
	sm->ExecuteStatement("Global.numSlides = function() { "
				//"if(!Global.ppPres) {throw new Error('No file open');}"
				"return Global.ppPres.Slides.Count;"
				"}");
	
	// closeFile()
	sm->ExecuteStatement("Global.closeFile = function() { "
				"if(Global.ppPres) {Global.ppPres.Close(); Global.ppPres = 0}"
				"}");
	
	// closePPT()
	sm->ExecuteStatement("Global.closePPT = function() { "
				"if(Global.ppPres) {Global.closeFile();}"
				"if(Global.ppApp)  {Global.ppApp.Quit(); Global.ppApp = 0}"
				"}");
	
	// showRun(int x, int y, int w, int h)
	sm->ExecuteStatement("Global.showRun = function(x,y,w,h) { "
				"if(!Global.ppPres) {throw new Error('No file open');}"
				"if(!Global.ppShowWin) {Global.ppShowWin = Global.ppPres.SlideShowSettings.Run();}"
				"if(x!=-1 || h!=-1 || w!=-1 || h!=-1) {"
					"Global.ppShowWin.Top=y;"
					"Global.ppShowWin.Left=x;"
					"Global.ppShowWin.Width=w;"
					"Global.ppShowWin.Height=h;"
					"}"
				"Global.ppShowWin.Activate()"
				"}");
	
	// showGotoSlide(int num)
	sm->ExecuteStatement("Global.gotoSlide = function(num) { "
				"if(!Global.ppPres) {throw new Error('No file open');}"
				"if(!Global.ppShowWin) {Global.showRun();}"
				"Global.ppShowWin.View.GotoSlide(num);"
				"}");
	
	// showExit()
	sm->ExecuteStatement("Global.showExit = function() { "
				"if(!Global.ppPres) {return;}"
				"if(!Global.ppShowWin) {return;}"
				"Global.ppShowWin.View.Exit();"
				"Global.ppShowWin = 0;"
				"}");
	//sm->ExecuteStatement("Global.ppPres = Global.ppApp.Presentations.Open(\"c:/test.ppt\");");
	//QVariant v = sm->Eval("Global.ppPres.Slides.Count");
	//qDebug() << "v:"<<v;
	qDebug() << "PPTLoader: Script Setup Complete";
#endif

}

void PPTLoader::openPPT()
{
#ifdef WIN32_PPT_ENABLED
	//qDebug() << "PPTLoader::openPPT";
	m_script->ExecuteStatement("Global.openPPT()");
#endif
}

void PPTLoader::openFile(const QString& file)
{
#ifdef WIN32_PPT_ENABLED
#endif
	qDebug() << "PPTLoader::openFile: "<<file;
	m_script->ExecuteStatement(QString("Global.openFile('%1')").arg(file));

	m_numSlides = m_script->Eval("Global.numSlides()").toInt();;
	if(m_numSlides > 0)
	{
		qDebug() << "PPTLoader::openFile: No hack needed, got num slide: "<<m_numSlides;
		return;
	}

	// Why in the WORLD am I calling the PowerPoint*VIEWER* when I've got a perfectly good OLE connection
	// via MSScriptControl to PowerPoint itself???
	// Very good and VERY frustrating question. You see, in all the tests I've ran, the Global.numSlides()
	// routine works fine - returns num slides. However, when integrated into DViz and ONLY when opening
	// a PowerPoint file AFTER starting Dviz, the Global.numSlides() routine returns 0. WHY?? Dont know.
	// So, if we cant get the num slides the right way, we go back door and use the viewer to tell us.

	PowerPointViewer::Application ppt;

	PowerPointViewer::SlideShowView * show =
		ppt.NewShow(file,
			PowerPointViewer::ppViewerSlideShowUseSlideTimings,
			PowerPointViewer::ppVFalse
		);

	m_numSlides = show->SlidesCount();
	qDebug() << "PPTLoader::openFile(): Hack needed, Num slides in "<<file<<": "<<m_numSlides;

	show->Exit();
	ppt.Quit();


}

void PPTLoader::closeFile()
{
#ifdef WIN32_PPT_ENABLED
	qDebug() << "PPTLoader::closeFile";
	m_script->ExecuteStatement("Global.closeFile()");
#endif
}
int  PPTLoader::numSlides()
{
#ifdef WIN32_PPT_ENABLED
	//qDebug() << "PPTLoader::numSlides";
	int val = m_script->Eval("Global.numSlides()").toInt();
	if(val > 0)
	    return val;
	return m_numSlides;
#endif
	return -1;
}
void PPTLoader::showRun(int x, int y, int w, int h)
{
#ifdef WIN32_PPT_ENABLED
	//qDebug() << "PPTLoader::showRun: "<<x<<y<<w<<h;
	m_script->ExecuteStatement(QString("Global.showRun(%1,%2,%3,%4)").arg(x).arg(y).arg(w).arg(h));
#endif
}
void PPTLoader::gotoSlide(int num)
{
#ifdef WIN32_PPT_ENABLED
	//qDebug() << "PPTLoader::gotoSlide: "<<num;
	m_script->ExecuteStatement(QString("Global.gotoSlide(%1)").arg(num));
#endif
}
void PPTLoader::showExit()
{
#ifdef WIN32_PPT_ENABLED
	qDebug() << "PPTLoader::showExit";
	m_script->ExecuteStatement("Global.showExit()");
#endif
}
void PPTLoader::closePPT()
{
#ifdef WIN32_PPT_ENABLED
	qDebug() << "PPTLoader::closePPT";
	m_script->ExecuteStatement("Global.closePPT()");
#endif
}
