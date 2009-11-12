#ifndef SLIDEGROUP_H
#define SLIDEGROUP_H

#include <QList>
class Slide;
class QFileInfo;
#include "AbstractVisualItem.h"

class SlideGroup : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int groupId READ groupId WRITE setGroupId);
	Q_PROPERTY(int groupNumber READ groupNumber WRITE setGroupNumber);
	Q_PROPERTY(GroupType groupType READ groupType WRITE setGroupType);
	Q_PROPERTY(QString groupTitle READ groupTitle WRITE setGroupTitle);
	Q_PROPERTY(QString iconFile READ iconFile WRITE setIconFile);
	Q_PROPERTY(bool autoChangeGroup READ autoChangeGroup WRITE setAutoChangeGroup);
	Q_PROPERTY(double inheritFadeSettings READ inheritFadeSettings WRITE setInheritFadeSettings);
	Q_PROPERTY(double crossFadeSpeed READ crossFadeSpeed WRITE setCrossFadeSpeed);
	Q_PROPERTY(double crossFadeQuality READ crossFadeQuality WRITE setCrossFadeQuality);


public:
	SlideGroup();
	virtual ~SlideGroup();

	typedef enum GroupType
	{
		Generic,
		SectionHeader,
		Song,
		Video,
		Feed,
		PowerPoint,
	};

	ITEM_PROPDEF(GroupId,		int,		groupId);
	ITEM_PROPDEF(GroupNumber,	int,		groupNumber);
	V_ITEM_PROPDEF(GroupType,	GroupType,	groupType);
	ITEM_PROPDEF(GroupTitle,	QString,	groupTitle);
	ITEM_PROPDEF(IconFile,		QString,	iconFile);
	ITEM_PROPDEF(AutoChangeGroup,	bool,		autoChangeGroup);
	ITEM_PROPDEF(InheritFadeSettings,bool,		inheritFadeSettings);
	ITEM_PROPDEF(CrossFadeSpeed,	double,		crossFadeSpeed);    // secs
	ITEM_PROPDEF(CrossFadeQuality,	double,		crossFadeQuality);  // frames

	void addSlide(Slide *);
	QList<Slide *> slideList();
	int numSlides() { return m_slides.size(); }
	Slide * at(int sortedIdx) { return sortedIdx < m_slides.size() ? m_slides.at(sortedIdx) : 0; }
	int indexOf(Slide *slide) { return m_slides.indexOf(slide); }

	void removeSlide(Slide *);

	virtual bool fromXml(QDomElement & parentElement);
	virtual void toXml(QDomElement & parentElement) const;
	
	virtual QByteArray toByteArray() const;
	//virtual void fromByteArray(QByteArray &);
	static SlideGroup * fromByteArray(QByteArray &);
	
	SlideGroup * clone();

	virtual void load(const QString & filename);
	virtual void save(const QString & filename = "");

	QString filename() const { return m_filename; }
	void setFilename(const QString& f) { m_filename=f; }

	virtual void changeBackground(AbstractVisualItem::FillType fillType, QVariant fillValue, Slide *onlyThisSlide = 0);
	virtual bool changeBackground(const QFileInfo & info, Slide *onlyThisSlide = 0);

	static bool canUseBackground(const QFileInfo & info);

	virtual Slide * masterSlide(bool autoCreate=true);

	QString assumedName();

signals:
	// Operation = "Add", "Remove", "Change"
	void slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);

private slots:
	void slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value, QVariant old);

protected:
	virtual void loadVariantMap(QVariantMap &);
	virtual void loadProperties(QVariantMap &);
	virtual void loadSlides(QVariantMap &);
	
	void saveProperties(QVariantMap&) const;
	void saveSlideList(QVariantMap&)  const;
	
	void loadGroupAttributes(QDomElement & parentElement);
	void loadSlideList(QDomElement & parentElement);

	void saveGroupAttributes(QDomElement & parentElement) const;
	void saveSlideList(QDomElement & parentElement) const;

	QList<Slide *> m_slides;
	int m_groupNumber;
	int m_groupId;
	GroupType m_groupType;
	QString m_groupTitle;
	QString m_iconFile;
	bool m_autoChangeGroup;

	void sortSlides();

	bool m_inheritFadeSettings;
	double m_crossFadeSpeed;
	double m_crossFadeQuality;

	Slide * m_masterSlide;

	QString m_filename;



};

#endif
