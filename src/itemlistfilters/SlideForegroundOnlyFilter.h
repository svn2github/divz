#ifndef SlideForegroundOnlyFilter_h
#define SlideForegroundOnlyFilter_h

#include "model/AbstractItemListFilter.h"

class SlideForegroundOnlyFilter : public AbstractItemListFilter
{
private:
	SlideForegroundOnlyFilter();
	static SlideForegroundOnlyFilter * m_staticInstance;
	
public:
	static SlideForegroundOnlyFilter * instance() { return m_staticInstance; }
	
	QString filterName() const { return "No Background"; }
	QString filterDescription() const;
	bool approve(AbstractItem *item);
};

#endif
