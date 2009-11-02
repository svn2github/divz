#ifndef SIMPLELOGGER_H
#define SIMPLELOGGER_H

class SimpleLogger
{
public:
	SimpleLogger()
	virtual ~SimpleLogger();
	virtual void log(const QString&, int severity = -1);

};

#endif
