#ifndef VIDEOFILECONFIG_H
#define VIDEOFILECONFIG_H

#include "GenericItemConfig.h"
class VideoFileContent;

class VideoFileConfig : public GenericItemConfig
{
    public:
        VideoFileConfig(VideoFileContent * v, QGraphicsItem * parent = 0);
        ~VideoFileConfig();

    private:
        // ::AbstractConfig
        void slotOkClicked();
        VideoFileContent  * m_vid;
};

#endif
