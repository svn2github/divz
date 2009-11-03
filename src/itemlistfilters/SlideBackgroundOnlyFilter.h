#ifndef SlideBackgroundOnlyFilter_h
#define SlideBackgroundOnlyFilter_h

#include "model/AbstractItemFilter.h"

class SlideBackgroundOnlyFilter : public AbstractItemFilter
{
	Q_OBJECT
protected:
	SlideBackgroundOnlyFilter();
	static SlideBackgroundOnlyFilter * m_staticInstance;
	
public:
	static SlideBackgroundOnlyFilter * instance() { return m_staticInstance; }
	
	enum { FilterId = 0x02 };
	int filterId() { return FilterId; }
	
	QString filterName() const { return "Background Only"; }
	QString filterDescription() const;
	bool approve(AbstractItem *item);
};

#endif
