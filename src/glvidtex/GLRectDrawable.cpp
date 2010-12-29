#include "GLRectDrawable.h"
#include "GLWidget.h"


GLRectDrawable::GLRectDrawable(QObject *parent)
	: GLDrawable(parent)
	, m_fillColor(Qt::white)
	, m_borderColor(Qt::black)
	, m_borderWidth(3.0)
	, m_cornerRounding(0)
{
	foreach(CornerItem *corner, m_cornerItems)
		corner->setDefaultLeftOp(CornerItem::Scale);
}

void GLRectDrawable::setFillColor(QColor c)
{
	m_fillColor = c;
	updateGL();
}

void GLRectDrawable::setBorderColor(QColor c)
{
	m_borderColor = c;
	updateGL();
}

void GLRectDrawable::setBorderWidth(double d)
{
	m_borderWidth = d;
	updateGL();
}

void GLRectDrawable::setCornerRounding(double d)
{
	m_cornerRounding = d;
	updateGL();
}

void GLRectDrawable::initGL()
{
	// noop
}

void GLRectDrawable::paintGL()
{
	glLoadIdentity(); // Reset The View
	QTransform transform =  m_glw->transform(); //= painter.deviceTransform();
	QPolygonF points = transform.map(QPolygonF(rect()));
	
// 	qDebug() << "GLRectDrawable::paintGL(): rect:"<<rect()<<", points:"<<points;
	
	//glColor4f(m_fillColor.redF(), m_fillColor.greenF(), m_fillColor.blueF(),opacity());
	glColor3f(0.5f,0.5f,1.0f);				// Set The Color To Blue One Time Only
	glBegin(GL_QUADS);					// Start Drawing Quads
// 		glVertex3f(-1.0f, 1.0f, 0.0f);			// Left And Up 1 Unit (Top Left)
// 		glVertex3f( 1.0f, 1.0f, 0.0f);			// Right And Up 1 Unit (Top Right)
// 		glVertex3f( 1.0f,-1.0f, 0.0f);			// Right And Down One Unit (Bottom Right)
// 		glVertex3f(-1.0f,-1.0f, 0.0f);			// Left And Down One Unit (Bottom Left)
		/*glTexCoord2f(txLeft, txBottom); 	*/glVertex3f(points[2].x(),points[2].y(),  0.0f); // top left
		/*glTexCoord2f(txRight, txBottom); 	*/glVertex3f(points[1].x(),points[1].y(),  0.0f); // top right
		/*glTexCoord2f(txRight, txTop); 	*/glVertex3f(points[0].x(),points[0].y(),  0.0f); // bottom right
		/*glTexCoord2f(txLeft, txTop); 		*/glVertex3f(points[3].x(),points[3].y(),  0.0f); // bottom left
	glEnd();						// Done Drawing A Quad
	
	if(m_borderWidth > 0.0)
	{
		glLineWidth(m_borderWidth);
		glColor4f(m_borderColor.redF(), m_borderColor.greenF(), m_borderColor.blueF(),opacity());
		glBegin(GL_LINES);
		
			// line from bottom right to top right
			glVertex3f(points[0].x(),points[0].y(), 0.f);
			glVertex3f(points[1].x(),points[1].y(), 0.f);
			
			// line from top right to top left
			glVertex3f(points[1].x(),points[1].y(), 0.f);
			glVertex3f(points[2].x(),points[2].y(), 0.f);
			
			// line from top left bottom left
			glVertex3f(points[2].x(),points[2].y(), 0.f);
			glVertex3f(points[3].x(),points[3].y(), 0.f);
			
			// line from bottom left to bottom right
			glVertex3f(points[3].x(),points[3].y(), 0.f);
			glVertex3f(points[0].x(),points[0].y(), 0.f);
			
		glEnd();
		glLineWidth(1.f); //reset
		
	}
}

void GLRectDrawable::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
	painter->setOpacity(opacity());
	painter->setPen(QPen(m_borderColor,m_borderWidth));
	painter->setBrush(m_fillColor);
	QRectF target = QRectF(QPointF(0,0),rect().size());
	painter->drawRect(target);
}
