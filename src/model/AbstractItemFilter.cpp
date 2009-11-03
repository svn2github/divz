#include "AbstractItemFilter.h"

QHash<int,AbstractItemFilter*> AbstractItemFilter::m_filtersById;

AbstractItemFilterList AbstractItemFilter::m_availableFilters = AbstractItemFilterList();

AbstractItemFilterList AbstractItemFilter::availableFilters() { return m_availableFilters; }
	
void AbstractItemFilter::registerFilterInstance(AbstractItemFilter *filter)
{
	if(!filter)
		return;
	m_availableFilters.append(filter);
	m_filtersById[filter->filterId()] = filter;
}

void AbstractItemFilter::removeFilterInstance(AbstractItemFilter *filter)
{
	m_availableFilters.removeAll(filter);
	m_filtersById.remove(filter->filterId());
}

AbstractItemFilter * AbstractItemFilter::filterById(int id)
{
	if(m_filtersById.contains(id))
		return m_filtersById[id];
	else
		return 0;
}


AbstractItemFilter::AbstractItemFilter() {}
AbstractItemFilter::~AbstractItemFilter() {}
/*
AbstractItemList AbstractItemFilter::filter(const AbstractItemList& list)
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
}*/

bool AbstractItemFilter::approve(AbstractItem*)
{
	return true;
}

AbstractItem * AbstractItemFilter::mutate(const AbstractItem *)
{
	return 0;
}
