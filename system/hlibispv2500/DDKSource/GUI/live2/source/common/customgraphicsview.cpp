#include "customgraphicsview.hpp"

#include <QCloseEvent>
#include <QWheelEvent>
#include <QGraphicsPixmapItem>

//
// Public Functions
//

VisionLive::CustomGraphicsView::CustomGraphicsView(QGraphicsView *parent): QGraphicsView(parent)
{
	_ratio = Qt::KeepAspectRatio;

	_enableZoom = true;
    _currentZoom = 1.0;
    _minZoom = 0.1;
    _maxZoom = 5;
    _stepZoom = 0.1;
}

VisionLive::CustomGraphicsView::~CustomGraphicsView()
{
}

void VisionLive::CustomGraphicsView::setAspectRatio(Qt::AspectRatioMode ratio)
{
	_ratio = ratio;
}

void VisionLive::CustomGraphicsView::setZoomingEnabled(bool zooming)
{
	_enableZoom = zooming;
}

//
// Protected Functions
//

void VisionLive::CustomGraphicsView::resizeEvent(QResizeEvent *event)
{
	if(!_enableZoom )
    {
        fitInView(0, 0, scene()->width(), scene()->height(), _ratio);
    }
    else
    {
        fitInView(0, 0, scene()->width(), scene()->height(), _ratio);
		setTransformationAnchor(QGraphicsView::AnchorViewCenter);
        scale(_currentZoom, _currentZoom);
    }

	event->accept();
}

void VisionLive::CustomGraphicsView::wheelEvent(QWheelEvent *event)
{
	double wantedZoom = _currentZoom;
    
    if(!_enableZoom)
    {
        return;
    }

    if(event->delta() > 0)
    {
        wantedZoom += _stepZoom;
    }
    else
    {
        wantedZoom -= _stepZoom;
    }

    if(wantedZoom < _minZoom)
    {
        wantedZoom = _minZoom;
    }
    if(wantedZoom > _maxZoom)
    {
        wantedZoom = _maxZoom;
    }

    if(_enableZoom)
    {
        setTransformationAnchor(QGraphicsView::NoAnchor);
        scale(wantedZoom/_currentZoom, wantedZoom/_currentZoom);
        _currentZoom = wantedZoom;
	}

	event->accept();
}

void VisionLive::CustomGraphicsView::mousePressEvent(QMouseEvent *event)
{
	emit mousePressed(event->pos(), event->button());

	event->accept();
}
