#ifndef BOXCONFIG_H
#define BOXCONFIG_H

#include "GenericItemConfig.h"
class BoxContent;

class BoxConfig : public GenericItemConfig
{
    public:
        BoxConfig(BoxContent * v, QGraphicsItem * parent = 0);
        ~BoxConfig();

    private:
        // ::AbstractConfig
        void slotOkClicked();
};

#endif
