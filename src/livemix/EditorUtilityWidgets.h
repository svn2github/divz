#ifndef EditorUtilityWidgets_H
#define EditorUtilityWidgets_H

#include <QtGui>

class PropertyEditorFactory : public QObject
{
	Q_OBJECT
public:
		
	class PropertyEditorOptions
	{
	public:
		PropertyEditorOptions()
		{
			reset();
		};

		void reset()
		{
			text = "";
			suffix = "";
			noSlider = false;
			min = -9999;
			max =  9999;
			type = QVariant::Invalid;
			value = QVariant();
			doubleIsPercentage = false;
			defaultValue = QVariant();
			stringIsFile = false;
			fileTypeFilter = "";
			step = 1;
		}

		QString text;
		QString suffix;
		bool noSlider;
		double min;
		double max;
		int step;
		QVariant::Type type;
		QVariant value;
		bool doubleIsPercentage;
		QVariant defaultValue;
		bool stringIsFile;
		QString fileTypeFilter;
	};

	static QWidget * generatePropertyEditor(QObject *object, const char *property, const char *slot, PropertyEditorOptions opts = PropertyEditorOptions(), const char *changeSignal=0);

};

class ObjectValueSetter : public QObject
{
	Q_OBJECT
public:
	ObjectValueSetter(QObject *attached, const char *slot, QVariant value);
	
public slots:
	void executeSetValue();
	
signals:
	void setValue(int);
	void setValue(double);
	void setValue(const QString&);

private:
	QVariant m_value;
};

class BrowseDialogLauncher : public QObject
{
	Q_OBJECT
public:
	BrowseDialogLauncher(QObject *attached, const char *slot, QVariant value);
	
public slots:
	void browse();
	
	void setTitle(const QString&);
	void setSettingsKey(const QString&);
	void setFilter(const QString&);
	
signals:
	void setValue(const QString&);

private:
	QObject * m_attached;
	QVariant m_value;
	QString m_settingsKey;
	QString m_title;
	QString m_filter;
};


class QSpinBox;
class QDoubleSpinBox;

class PropertyChangeListener : public QObject
{
	Q_OBJECT
public:
	PropertyChangeListener(QObject *source, const char *changeSignal, QObject *receiver, const char *receiverSlot, QVariant value, QString propertyName = "");
	
signals:
	void value(int);
	void value(bool);
	void value(double);
	void value(const QString&);
	void value(const QSize&);
	void value(const QSizeF&);
	void value(const QPoint&);
	void value(const QPointF&);
	
private slots:
	void receiver(const QString&, const QVariant&);
	void receiver(const QVariant&);
	
private:
	QVariant m_value;
	QString m_property;
};


class DoubleEditorWidget : public QWidget
{
	Q_OBJECT
public:
	DoubleEditorWidget(QWidget *parent=0);

public slots:
	void setValue(double);
	void setMinMax(double,double);
	void setShowSlider(bool);
	void setSuffix(const QString&);

signals:
	void valueChanged(double);

private slots:
	void sliderValueChanged(int);
	void boxValueChanged(double);

private:
	double m_value;
	QSlider *m_slider;
	QDoubleSpinBox *m_box;

};

class PointEditorWidget : public QWidget
{
	Q_OBJECT
public:
	PointEditorWidget(QWidget *parent=0);

public slots:
	void setValue(const QPointF&);
	void setXMinMax(int,int);
	void setYMinMax(int,int);
	void setSufix(const QString&);
	
	void reset();

signals:
	void valueChanged(const QPointF&);

private slots:
	void xValueChanged(int);
	void yValueChanged(int);

private:
	QPointF m_point;
	QSpinBox *x_box;
	QSpinBox *y_box;
	QPointF m_orig;

};

class SizeEditorWidget : public QWidget
{
	Q_OBJECT
public:
	SizeEditorWidget(QWidget *parent=0);

public slots:
	void setValue(const QSizeF&);
	void setWMinMax(int,int);
	void setHMinMax(int,int);
	void setSufix(const QString&);
	
	void reset();

signals:
	void valueChanged(const QSizeF&);

private slots:
	void wValueChanged(int);
	void hValueChanged(int);


private:
	QSizeF m_size;
	QSpinBox *w_box;
	QSpinBox *h_box;
	QSizeF m_orig;
};
/*
class ColorEditorWidget : public QWidget
{
	Q_OBJECT
public:
	ColorEditorWidget(QWidget *parent=0);

public slots:
	void setValue(const QColor&);

signals:
	void valueChanged(const QColor&);

private slots:
	void rValueChanged(int);
	void gValueChanged(int);
	void bValueChanged(int);

private:
	QColor m_point;
	QSpinBox *r_box;
	QSpinBox *g_box;
	QSpinBox *b_box;
};*/


class PositionWidget : public QWidget
{
	Q_OBJECT
public:
	PositionWidget(class LiveLayer *layer);

signals:
// 	void 

private slots:
	void setLayerY(double);
	void setLayerX(double);
	
	void setLayerWidth(double);
	void setLayerHeight(double);
	
	void setARLeft(int);
	void setARRight(int);
	
	void layerPropertyChanged(const QString& propertyId, const QVariant& value, const QVariant& oldValue);
	
	void setAspectRatioIdx(int);
	
private:
	double heightFromWidth(double);
	double widthFromHeight(double);
	
	LiveLayer *m_layer;
	bool m_lockAspectRatio;
	double m_lockToAR;
	
	QSpinBox *m_arLeft;
	QSpinBox *m_arRight;
	QDoubleSpinBox *m_posY;
	QDoubleSpinBox *m_posX;
	QDoubleSpinBox *m_sizeWidth;
	QDoubleSpinBox *m_sizeHeight;
	
	QList<QPoint> m_arList;
	
	bool m_lockValueUpdates;
};

#endif

