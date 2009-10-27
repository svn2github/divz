#include "SlideTextOnlyFilter.h"

#include "model/BackgroundItem.h"

SlideTextOnlyFilter * SlideTextOnlyFilter::m_staticInstance = new SlideTextOnlyFilter();
SlideTextOnlyFilter::SlideTextOnlyFilter()
{
	m_backgroundColor = Qt::black;
	//AbstractItemListFilter::registerFilterInstance(this); 
}

QString SlideTextOnlyFilter::filterDescription() const { return "Only text items are shown"; }
bool SlideTextOnlyFilter::approve(AbstractItem *item)  { return item && item->inherits("TextBoxItem"); }

AbstractItem * SlideTextOnlyFilter::mutate(const AbstractItem * sourceItem)
{
	if(sourceItem && sourceItem->inherits("BackgroundItem"))
	{
		AbstractItem * clone = sourceItem->clone();
		AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(clone);
		bg->setFillType(AbstractVisualItem::Solid);
		bg->setFillBrush(m_backgroundColor);
		
		return clone;
	}
	
	return 0;
}

void SlideTextOnlyFilter::setBackgroundColor(const QColor & bg)
{
	m_backgroundColor = bg;
}