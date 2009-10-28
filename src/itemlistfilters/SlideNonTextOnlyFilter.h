#ifndef SlideNonTextOnlyFilter_h
#define SlideNonTextOnlyFilter_h

#include "model/AbstractItemFilter.h"

class SlideNonTextOnlyFilter : public AbstractItemFilter
{
	Q_OBJECT
protected:
	SlideNonTextOnlyFilter();
	static SlideNonTextOnlyFilter * m_staticInstance;
	
public:
	static SlideNonTextOnlyFilter * instance() { return m_staticInstance; }
	
	QString filterName() const { return "Non-Text Items"; }
	QString filterDescription() const;
	
	bool approve(AbstractItem *item);
};

#endif
