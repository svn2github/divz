#ifndef SlideBackgroundOnlyFilter_h
#define SlideBackgroundOnlyFilter_h

#include "model/AbstractItemListFilter.h"

class SlideBackgroundOnlyFilter : public AbstractItemListFilter
{
private:
	SlideBackgroundOnlyFilter();
	static SlideBackgroundOnlyFilter * m_staticInstance;
	
public:
	static SlideBackgroundOnlyFilter * instance() { return m_staticInstance; }
	
	QString filterName() const { return "Background Only"; }
	QString filterDescription() const;
	bool approve(AbstractItem *item);
};

#endif