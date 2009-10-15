#ifndef IMAGECONFIG_H
#define IMAGECONFIG_H

#include "GenericItemConfig.h"
class ImageContent;

class ImageConfig : public GenericItemConfig
{
    public:
        ImageConfig(ImageContent * v, QGraphicsItem * parent = 0);
        ~ImageConfig();

    private:
        // ::AbstractConfig
        void slotOkClicked();
};

#endif
