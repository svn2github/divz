#ifndef IMAGEFILTERS_H
#define IMAGEFILTERS_H

#include <QImage>

class ImageFilters
{
public:
	static QImage blurred(const QImage& image, const QRect& rect, int radius);

};
#endif
