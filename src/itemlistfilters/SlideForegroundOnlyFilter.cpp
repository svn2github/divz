#include "SlideForegroundOnlyFilter.h"

SlideForegroundOnlyFilter::SlideForegroundOnlyFilter()  {} //AbstractItemListFilter::registerFilterInstance(this); }
SlideForegroundOnlyFilter * SlideForegroundOnlyFilter::m_staticInstance = new SlideForegroundOnlyFilter();

QString SlideForegroundOnlyFilter::filterDescription() const { return "No background is shown (background appears black)"; }
bool SlideForegroundOnlyFilter::approve(AbstractItem *item)  { return item && ! item->inherits("BackgroundItem"); }