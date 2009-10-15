void QPixmapDropShadowFilter::draw(QPainter *p,
                                   const QPointF &pos,
                                   const QPixmap &px,
                                   const QRectF &src) const
{
    Q_D(const QPixmapDropShadowFilter);
    QPixmapFilter *filter = p->paintEngine() && p->paintEngine()->isExtended() ?
        static_cast<QPaintEngineEx *>(p->paintEngine())->pixmapFilter(type(), this) : 0;
    QPixmapDropShadowFilter *dropShadowFilter = static_cast<QPixmapDropShadowFilter*>(filter);
    if (dropShadowFilter) {
        dropShadowFilter->setColor(d->color);
        dropShadowFilter->setBlurRadius(d->blurFilter->radius());
        dropShadowFilter->setOffset(d->offset);
        dropShadowFilter->draw(p, pos, px, src);
        return;
    }

    QImage tmp = src.isNull() ? px.toImage() : px.copy(src.toRect()).toImage();
    QPainter tmpPainter(&tmp);

    // blacken the image...
    tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tmpPainter.fillRect(0, 0, tmp.width(), tmp.height(), d->color);
    tmpPainter.end();

    const QPixmap pixTmp = QPixmap::fromImage(tmp);

    // draw the blurred drop shadow...
    d->blurFilter->draw(p, pos + d->offset, pixTmp);

    // Draw the actual pixmap...
    p->drawPixmap(pos, px, src);
}




// Blur the image according to the blur radius
// Based on exponential blur algorithm by Jani Huhtanen
// (maximum radius is set to 16)
static QImage blurred(const QImage& image, const QRect& rect, int radius)
{
    int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
    int alpha = (radius < 1)  ? 16 : (radius > 17) ? 1 : tab[radius-1];

    QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int r1 = rect.top();
    int r2 = rect.bottom();
    int c1 = rect.left();
    int c2 = rect.right();

    int bpl = result.bytesPerLine();
    int rgba[4];
    unsigned char* p;

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r1) + col * 4;
        for (int i = 0; i < 4; i++)
            rgba[i] = p[i] << 4;

        p += bpl;
        for (int j = r1; j < r2; j++, p += bpl)
            for (int i = 0; i < 4; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c1 * 4;
        for (int i = 0; i < 4; i++)
            rgba[i] = p[i] << 4;

        p += 4;
        for (int j = c1; j < c2; j++, p += 4)
            for (int i = 0; i < 4; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r2) + col * 4;
        for (int i = 0; i < 4; i++)
            rgba[i] = p[i] << 4;

        p -= bpl;
        for (int j = r1; j < r2; j++, p -= bpl)
            for (int i = 0; i < 4; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c2 * 4;
        for (int i = 0; i < 4; i++)
            rgba[i] = p[i] << 4;

        p -= 4;
        for (int j = c1; j < c2; j++, p -= 4)
            for (int i = 0; i < 4; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    return result;
}