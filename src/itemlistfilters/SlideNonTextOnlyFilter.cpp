#include "SlideNonTextOnlyFilter.h"

SlideNonTextOnlyFilter::SlideNonTextOnlyFilter()  {} // AbstractItemListFilter::registerFilterInstance(this); }
SlideNonTextOnlyFilter * SlideNonTextOnlyFilter::m_staticInstance  = new SlideNonTextOnlyFilter();

QString SlideNonTextOnlyFilter::filterDescription() const { return "Only non-text items are shown"; }
bool SlideNonTextOnlyFilter::approve(AbstractItem *item)  { return item && ! item->inherits("TextBoxItem"); }
