#ifndef BACKGROUNDCONFIG_H
#define BACKGROUNDCONFIG_H

#include "GenericItemConfig.h"
class BackgroundContent;

class BackgroundConfig : public GenericItemConfig
{
    public:
        BackgroundConfig(BackgroundContent * v, QGraphicsItem * parent = 0);
        ~BackgroundConfig();

    private:
        // ::AbstractConfig
        void slotOkClicked();
        BackgroundContent *m_bg;
};

#endif
