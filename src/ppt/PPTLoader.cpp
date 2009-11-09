#include "PPTLoader.h"
#include <windows.h>
#include <QDebug>
#include <QFile>


//MSScriptControl::ScriptControl * PPTLoader::m_script = 0;

PPTLoader::PPTLoader()
	: QObject()
{
	setupScripts();
}

PPTLoader::~PPTLoader()
{
	showExit();
	closeFile();
	closePPT();
	delete m_script;
}



void PPTLoader::axException(int code,QString source,QString description,QString help)
{
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
}

void PPTLoader::setupScripts()
{
	//if(m_script)
	//	return;
    /*
  QFile file("/proc/modules");
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	 return;

     QTextStream in(&file);
     QString line = in.readLine();
     while (!line.isNull()) {
	 process_line(line);
	 line = in.readLine();
     }
     */
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

}

void PPTLoader::openPPT()
{
	//qDebug() << "PPTLoader::openPPT";
	m_script->ExecuteStatement("Global.openPPT()");
}

void PPTLoader::openFile(const QString& file)
{
	qDebug() << "PPTLoader::openFile: "<<file;
	m_script->ExecuteStatement(QString("Global.openFile('%1')").arg(file));
}

void PPTLoader::closeFile()
{
	qDebug() << "PPTLoader::closeFile";
	m_script->ExecuteStatement("Global.closeFile()");
}
int  PPTLoader::numSlides()
{
	//qDebug() << "PPTLoader::numSlides";
	return m_script->Eval("Global.numSlides()").toInt();
}
void PPTLoader::showRun(int x, int y, int w, int h)
{
	//qDebug() << "PPTLoader::showRun: "<<x<<y<<w<<h;
	m_script->ExecuteStatement(QString("Global.showRun(%1,%2,%3,%4)").arg(x).arg(y).arg(w).arg(h));
}
void PPTLoader::gotoSlide(int num)
{
	//qDebug() << "PPTLoader::gotoSlide: "<<num;
	m_script->ExecuteStatement(QString("Global.gotoSlide(%1)").arg(num));
}
void PPTLoader::showExit()
{
	qDebug() << "PPTLoader::showExit";
	m_script->ExecuteStatement("Global.showExit()");
}
void PPTLoader::closePPT()
{
	qDebug() << "PPTLoader::closePPT";
	m_script->ExecuteStatement("Global.closePPT()");
}
