#ifndef SlideForegroundOnlyFilter_h
#define SlideForegroundOnlyFilter_h

#include "model/AbstractItemFilter.h"

class SlideForegroundOnlyFilter : public AbstractItemFilter
{
	Q_OBJECT
protected:
	SlideForegroundOnlyFilter();
	static SlideForegroundOnlyFilter * m_staticInstance;
	
public:
	static SlideForegroundOnlyFilter * instance() { return m_staticInstance; }
	
	enum { FilterId = 0x03 };
	int filterId() { return FilterId; }
	
	QString filterName() const { return "No Background"; }
	QString filterDescription() const;
	bool approve(AbstractItem *item);
};

#endif
