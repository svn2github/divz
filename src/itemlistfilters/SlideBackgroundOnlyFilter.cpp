#include "SlideBackgroundOnlyFilter.h"

SlideBackgroundOnlyFilter::SlideBackgroundOnlyFilter() {} // AbstractItemListFilter::registerFilterInstance(this); }
SlideBackgroundOnlyFilter * SlideBackgroundOnlyFilter::m_staticInstance = new SlideBackgroundOnlyFilter();
	
QString SlideBackgroundOnlyFilter::filterDescription() const { return "Only the background is shown."; }
bool SlideBackgroundOnlyFilter::approve(AbstractItem *item)  { return item && item->inherits("BackgroundItem"); }
