#include "customgraphicsview.hpp"

#include <QCloseEvent>
#include <QWheelEvent>
#include <QGraphicsPixmapItem>
#include <QScrollBar>

//
// Public Functions
//

VisionLive::CustomGraphicsView::CustomGraphicsView(QWidget *parent): QGraphicsView(parent)
{
	_ratio = Qt::KeepAspectRatio;

	_enableZoom = true;
    _currentZoom = 1.0;
    _minZoom = 1.0;
    _maxZoom = 10.0;
    _stepZoom = 0.2;

	_leftMouseButtonPressed = false;
	_rightMouseButtonPressed = false;
	_imageMoveStartPoint = QPoint(0, 0);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
		const QPointF p0scene = mapToScene(event->pos());

        scale(wantedZoom/_currentZoom, wantedZoom/_currentZoom);

		const QPointF p1mouse = mapFromScene(p0scene);
	    const QPointF move = p1mouse - event->pos();
	    horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
	    verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
        
		_currentZoom = wantedZoom;
	}

	event->accept();
}

void VisionLive::CustomGraphicsView::mousePressEvent(QMouseEvent *event)
{
	emit mousePressed(event->pos(), event->button());

	if(event->button() == Qt::LeftButton)
	{
		_leftMouseButtonPressed = true;

		emit mousePressed(event->pos(), event->button());
	}
	else if(event->button() == Qt::RightButton)
	{
		_rightMouseButtonPressed = true;

		_imageMoveStartPoint = mapToScene(event->pos()).toPoint();
	}
}

void VisionLive::CustomGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	if(_leftMouseButtonPressed)
	{
	}
	else if(_rightMouseButtonPressed)
	{
		const QPointF p1mouse = mapFromScene(_imageMoveStartPoint);
	    const QPointF move = p1mouse - event->pos();
	    horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
	    verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
	}
}

void VisionLive::CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		_leftMouseButtonPressed = false;
	}
	else if(event->button() == Qt::RightButton)
	{
		_rightMouseButtonPressed = false;
	}
}

