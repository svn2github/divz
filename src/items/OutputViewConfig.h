#ifndef OUTPUTVIEWCONFIG_H
#define OUTPUTVIEWCONFIG_H

#include "GenericItemConfig.h"
class OutputViewContent;
#include <QComboBox>

class OutputViewConfig : public GenericItemConfig
{
	Q_OBJECT
    public:
        OutputViewConfig(OutputViewContent * v, QGraphicsItem * parent = 0);
        ~OutputViewConfig();

	private slots:
		void slotOutputChanged(int);

    private:

        // ::AbstractConfig
        void slotOkClicked();
        QComboBox * m_syncWithBox;
};

#endif
