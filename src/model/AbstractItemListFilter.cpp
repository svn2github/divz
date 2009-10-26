#include "AbstractItemListFilter.h"

QList<AbstractItemListFilter *> AbstractItemListFilter::m_availableFilters = QList<AbstractItemListFilter *>();

QList<AbstractItemListFilter *> AbstractItemListFilter::availableFilters() { return m_availableFilters; }
	
void AbstractItemListFilter::registerFilterInstance(AbstractItemListFilter *filter)
{
	if(!filter)
		return;
	m_availableFilters.append(filter);
}

void AbstractItemListFilter::removeFilterInstance(AbstractItemListFilter *filter)
{
	m_availableFilters.removeAll(filter);
}


AbstractItemListFilter::AbstractItemListFilter() {}
AbstractItemListFilter::~AbstractItemListFilter() {}

AbstractItemList AbstractItemListFilter::filter(const AbstractItemList& list)
{
	AbstractItemList newList;
	foreach(AbstractItem *oldItem, list)
	{
		if(approve(oldItem))
		{
			AbstractItem * mutated = mutate(oldItem);
			newList.append(mutated ? mutated : oldItem);
		}
	}
	
	return newList;
}

bool AbstractItemListFilter::approve(AbstractItem*)
{
	return true;
}

AbstractItem * AbstractItemListFilter::mutate(const AbstractItem *)
{
	return 0;
}
