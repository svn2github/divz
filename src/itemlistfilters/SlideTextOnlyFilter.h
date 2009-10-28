#ifndef SlideTextOnlyFilter_h
#define SlideTextOnlyFilter_h

#include <QColor>
#include "model/AbstractItemFilter.h"

class SlideTextOnlyFilter : public AbstractItemFilter
{
	Q_OBJECT
protected:
	SlideTextOnlyFilter();
	static SlideTextOnlyFilter * m_staticInstance;
	
public:
	static SlideTextOnlyFilter * instance() { return m_staticInstance; }
	
	QString filterName() const { return "Text-Only"; }
	QString filterDescription() const;
	bool approve(AbstractItem *item);
	AbstractItem * mutate(const AbstractItem*);
	
	void setBackgroundColor(const QColor &);
	QColor backgroundColor() const { return m_backgroundColor; }

private:
	QColor m_backgroundColor;
};

#endif
