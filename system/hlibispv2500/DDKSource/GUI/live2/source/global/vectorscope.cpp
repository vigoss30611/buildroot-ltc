#include "vectorscope.hpp"

#include <QGraphicsPixmapItem>
#include <qevent.h>

//
// Public Functions
//

VisionLive::VectorScope::VectorScope(QGraphicsView *parent): QGraphicsView(parent)
{
	_pScene = NULL;
	_pSaturation100Item = NULL;
	_pSaturation60Item = NULL;
	_pSaturation20Item = NULL;

	setUpScene();

	//setUpVectorscope();
}

VisionLive::VectorScope::~VectorScope()
{
}

//
// Protected Functions
//

void VisionLive::VectorScope::resizeEvent(QResizeEvent *event)
{
    fitInView(0, 0, _pScene->width(), _pScene->height(), Qt::KeepAspectRatio);
	
	event->accept();
}

//
// Private Functions
//

void VisionLive::VectorScope::setUpScene()
{
	_pScene = new QGraphicsScene();
	setScene(_pScene);
}

void VisionLive::VectorScope::setUpVectorscope()
{
	int maxSaturation = 255;
	int saturation60 = maxSaturation*60/100;
	int saturation20 = maxSaturation*20/100;

	_pSaturation100Item = new QGraphicsEllipseItem(-maxSaturation, -maxSaturation, 2*maxSaturation, 2*maxSaturation);
	_pSaturation60Item = new QGraphicsEllipseItem(-saturation60, -saturation60, 2*saturation60, 2*saturation60);
	_pSaturation20Item = new QGraphicsEllipseItem(-saturation20, -saturation20, 2*saturation20, 2*saturation20);

	_pSaturation100Item->setPen(QPen(Qt::white));
	_pSaturation60Item->setPen(QPen(Qt::white));
	_pSaturation20Item->setPen(QPen(Qt::white));

	QGraphicsTextItem *_pSaturation100TextItem = new QGraphicsTextItem("100%");
	_pSaturation100TextItem->setPos(0 - _pSaturation100TextItem->boundingRect().width()/2, -maxSaturation);
	_pSaturation100TextItem->setDefaultTextColor(Qt::white);
	QGraphicsTextItem *_pSaturation60TextItem = new QGraphicsTextItem("60%");
	_pSaturation60TextItem->setPos(0 - _pSaturation60TextItem->boundingRect().width()/2, -saturation60);
	_pSaturation60TextItem->setDefaultTextColor(Qt::white);
	QGraphicsTextItem *_pSaturation20TextItem = new QGraphicsTextItem("20%");
	_pSaturation20TextItem->setPos(0 - _pSaturation20TextItem->boundingRect().width()/2, -saturation20);
	_pSaturation20TextItem->setDefaultTextColor(Qt::white);

	if(_pScene)
	{
		_pScene->addItem(_pSaturation100Item);
		_pScene->addItem(_pSaturation60Item);
		_pScene->addItem(_pSaturation20Item);

		_pScene->addItem(_pSaturation100TextItem);
		_pScene->addItem(_pSaturation60TextItem);
		_pScene->addItem(_pSaturation20TextItem);
	}
}

//
// Public Slots
//

void VisionLive::VectorScope::vectorscopeReceived(QMap<int, std::pair<int, QRgb> > points)
{
	if(_pScene && points.size() > 0)
	{
		if(points.size() != _vectorscopeLines.size())
		{
			// Remove old lines
			for(unsigned int i = 0; i < _vectorscopeLines.size(); i++)
			{
				_pScene->removeItem(_vectorscopeLines[i]);
				delete _vectorscopeLines[i];
				_vectorscopeLines[i] = NULL;
			}
			_vectorscopeLines.clear();
			
			// Create new lines
			QMap<int, std::pair<int, QRgb> >::const_iterator it;
			for(int i = 0; i < points.size(); i++)
			{
				it = points.begin()+i;
				QGraphicsLineItem *line = new QGraphicsLineItem(0, 0, 0, -it.value().first);
				//line->setPos(_pScene->width()/2, _pScene->height()/2);
				line->setRotation(-it.key());
				line->setPen(QPen(it.value().second));
				//line->setScale(std::min(_pScene->width(), _pScene->height())/200);
				_vectorscopeLines.push_back(line);
			}

			// Display new lines
			for(unsigned int i = 0; i < _vectorscopeLines.size(); i++)
			{
				_pScene->addItem(_vectorscopeLines[i]);
			}
		}
		else
		{
			// Update lines
			QMap<int, std::pair<int, QRgb> >::const_iterator it;
			for(int i = 0; i < points.size(); i++)
			{
				it = points.begin()+i;
				_vectorscopeLines[i]->setLine(0, 0, 0, -it.value().first);
				//_vectorscopeLines[i]->setPos(_pScene->width()/2, _pScene->height()/2);
				_vectorscopeLines[i]->setRotation(-it.key());
				_vectorscopeLines[i]->setPen(QPen(it.value().second));
				//_vectorscopeLines[i]->setScale(std::min(_pScene->width(), _pScene->height())/200);
			}
		}
	}
}

