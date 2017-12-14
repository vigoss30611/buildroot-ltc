#include "livefeedview.hpp"
#include "imagedata.hpp"

#ifdef WIN32
#define _USE_MATH_DEFINES // to define M_PI
#endif
#include <math.h>

#include <qmenu.h>
#include <QContextMenuEvent>
#include <QWheelEvent>
#include <qfiledialog.h>
#include <QGraphicsPixmapItem>
#include <QScrollBar>

//
// Public Functions
//

VisionLive::LiveFeedView::LiveFeedView(QWidget *parent): QGraphicsView(parent)
{
	_pScene = NULL;
	_pImageItem = NULL;
	_pImageImfoItem = NULL;
	_pFPSInfoItem = NULL;
	_pContextMenu = NULL;
	_pMarkerItem = NULL;
	_pPixelInfoItem = NULL;
	_pAvgItem = NULL;
	_pSNRItem = NULL;
	_pSelectionRect = NULL;
	_pLineViewLineItem = NULL;
	_pAWBItem = NULL;

	_lineViewOption = None;

	_selecting = false;
	_moving = false;
	_leftMouseButtonPressed = false;
	_rightMouseButtonPressed = false;
	_selectionStartPoint = QPoint(0, 0);
	_selectionEndPoint = QPoint(0, 0);
	_imageMoveStartPoint = QPoint(0, 0);

	_type = 0;
	_format = 0;
	_mosaic = 0;
	_width = 0;
	_height = 0;
	_firstFrameReceived = false;

	_fps = 0.0;

	_enableZoom = true;
    _currentZoom = 1.0;
    _minZoom = 1.0;
    _maxZoom = 10.0;
    _stepZoom = 0.2;

	_marker = QPoint(0, 0);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	initScene();

	initImageItem();

	initImageImfoItem();

	initFPSInfoItem();

	initContextMenu();

	initMarkerItem();

	initPixelInfoItem();

	initAreaStatisticsItems();

	initAWBItems();
}

VisionLive::LiveFeedView::~LiveFeedView()
{
}

void VisionLive::LiveFeedView::setLiveViewOption(int opt)
{
	if(opt == 1)
	{
		_lineViewOption = Horizontal;
		if(!_pLineViewLineItem && _pImageItem && _pScene)
		{
			_pLineViewLineItem = new QGraphicsLineItem(0, _marker.y(), _width, _marker.y());
			_pLineViewLineItem->setPen(QPen(QBrush(Qt::white), 1));
			_pScene->addItem(_pLineViewLineItem);
		}
		else if(_pLineViewLineItem && _pImageItem && _pScene)
		{
			_pLineViewLineItem->setLine(0, _marker.y(), _pImageItem->boundingRect().width(), _marker.y());
		}
	}
	else if(opt == 2) 
	{
		_lineViewOption = Vertical;
		if(!_pLineViewLineItem && _pImageItem && _pScene)
		{
			_pLineViewLineItem = new QGraphicsLineItem(_marker.x(), 0, _marker.x(), _height);
			_pLineViewLineItem->setPen(QPen(QBrush(Qt::white), 1));
			_pScene->addItem(_pLineViewLineItem);
		}
		else if(_pLineViewLineItem && _pImageItem && _pScene)
		{
			_pLineViewLineItem->setLine(_marker.x(), 0, _marker.x(), _pImageItem->boundingRect().height());
		}
	}
	else 
	{
		_lineViewOption = None;
		if(_pLineViewLineItem && _pScene)
		{
			_pScene->removeItem(_pLineViewLineItem);
			_pLineViewLineItem = NULL;
		}
	}
}

//
// Protected Functions
//

void VisionLive::LiveFeedView::resizeEvent(QResizeEvent *event)
{
	if(!_enableZoom )
    {
        fitInView(0, 0, _pScene->width(), _pScene->height(), Qt::KeepAspectRatio);
    }
    else
    {
        fitInView(0, 0, _pScene->width(), _pScene->height(), Qt::KeepAspectRatio);
        scale(_currentZoom, _currentZoom);
    }

	event->accept();
}

void VisionLive::LiveFeedView::contextMenuEvent(QContextMenuEvent *event)
{
	if(_pContextMenu)
	{
		_pContextMenu->popup(event->globalPos());
	}

	event->accept();
}

void VisionLive::LiveFeedView::wheelEvent(QWheelEvent *event)
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

void VisionLive::LiveFeedView::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		_leftMouseButtonPressed = true;

		QPointF point = mapToScene(event->pos());

		if(!(point.x() < 0 || point.x() > _width ||
			point.y() < 0 || point.y() > _height))
		{
			_selectionStartPoint = point.toPoint();
		}
	}
	else if(event->button() == Qt::RightButton)
	{
		_rightMouseButtonPressed = true;

		_imageMoveStartPoint = mapToScene(event->pos()).toPoint();
	}

	event->accept();
}

void VisionLive::LiveFeedView::mouseMoveEvent(QMouseEvent *event)
{
	if(_leftMouseButtonPressed)
	{
		_selecting = true;

		QPointF point = mapToScene(event->pos());

		if(!(point.x() < 0 || point.x() > _width ||
			point.y() < 0 || point.y() > _height))
		{
			_selectionEndPoint = point.toPoint();
		}

		if(!_pSelectionRect && _pScene)
		{
			_pSelectionRect = new QGraphicsRectItem();
			_pSelectionRect->setPen(QPen(QBrush(Qt::white), 1));
			_pScene->addItem(_pSelectionRect);
		}

		if(_pSelectionRect)
		{
			_pSelectionRect->setRect(_selectionStartPoint.x(), _selectionStartPoint.y(), _selectionEndPoint.x() - _selectionStartPoint.x(), _selectionEndPoint.y() - _selectionStartPoint.y());
		}
	}
	else if(_rightMouseButtonPressed)
	{
		_moving = true;
		const QPointF p1mouse = mapFromScene(_imageMoveStartPoint);
	    const QPointF move = p1mouse - event->pos();
	    horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
	    verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());
	}
}

void VisionLive::LiveFeedView::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		_leftMouseButtonPressed = false;
	}
	else if(event->button() == Qt::RightButton)
	{
		_rightMouseButtonPressed = false;
	}

	if(_selecting)
	{
		_selecting = false;

		if(_pSelectionRect)
		{
			if(_pSelectionRect->rect().width() < 0)
			{
				_pSelectionRect->setRect(_pSelectionRect->rect().x()+_pSelectionRect->rect().width(), _pSelectionRect->rect().y(), -_pSelectionRect->rect().width(), _pSelectionRect->rect().height());
			}

			if(_pSelectionRect->rect().height() < 0)
			{
				_pSelectionRect->setRect(_pSelectionRect->rect().x(), _pSelectionRect->rect().y()+_pSelectionRect->rect().height(), _pSelectionRect->rect().width(), -_pSelectionRect->rect().height());
			}

			emit CS_setSelection(_pSelectionRect->rect().toRect(), true);
		}
	}
	else if(_moving)
	{
		_moving = false;
	}
	else
	{
		if(event->button() == Qt::LeftButton)
		{
			QPointF point = mapToScene(event->pos());

			if(!(point.x() < 0 || point.x() > _width ||
				point.y() < 0 || point.y() > _height))
			{
				_marker = point;
				_pMarkerItem->setPos(_marker.x(), _marker.y());
				setLiveViewOption((int)_lineViewOption);
				emit CS_setMarker(_marker);
			}
		}
		else if(event->button() == Qt::RightButton)
		{
			if(_pScene && _pSelectionRect)
			{
				emit CS_setSelection(_pSelectionRect->rect().toRect(), false);

				_pScene->removeItem(_pSelectionRect);
				delete _pSelectionRect;
				_pSelectionRect = NULL;

				_pAvgItem->setPlainText(QString());
				_pSNRItem->setPlainText(QString());
			}
		}
	}

	event->accept();
}

//
// Private Functions
//

void VisionLive::LiveFeedView::initScene()
{
	_pScene = new QGraphicsScene();
	setScene(_pScene);
}

void VisionLive::LiveFeedView::initImageItem()
{
	_pImageItem = new QGraphicsPixmapItem();
	_pImageItem->setTransformationMode(Qt::SmoothTransformation);

	if(_pScene)
	{
		_pScene->addItem(_pImageItem);
	}
	else
	{
		emit logError(tr("Couldn't add ImageItem to Scene!"), tr("LiveFeedView::setUpImageItem()"));
	}
}

void VisionLive::LiveFeedView::initImageImfoItem()
{
	_pImageImfoItem = new QGraphicsTextItem();
	_pImageImfoItem->setPos(0, 0);
	_pImageImfoItem->setDefaultTextColor(Qt::white);
	//_pImageImfoItem->setHtml("<div style='background-color: rgba(0, 0, 0, 0.6);' ></div>");
}

void VisionLive::LiveFeedView::initFPSInfoItem()
{
	_pFPSInfoItem = new QGraphicsTextItem();
	_pFPSInfoItem->setDefaultTextColor(Qt::white);
	QString info = QString(tr("FPS: calculating..."));
	//_pFPSInfoItem->setHtml("<div style='background-color: rgba(0, 0, 0, 0.6);' >" + info + "</div>");
	_pFPSInfoItem->setPlainText(info);

	if(_pImageImfoItem)
	{
		_pFPSInfoItem->setPos(0, _pImageImfoItem->boundingRect().height());
	}
}

void VisionLive::LiveFeedView::initContextMenu()
{
}

void VisionLive::LiveFeedView::initMarkerItem()
{
	_pMarkerItem = new QGraphicsRectItem(-2, -2, 4, 4);
	_pMarkerItem->setPen(QPen(Qt::white));
	_pMarkerItem->setBrush(Qt::white);
}

void VisionLive::LiveFeedView::initPixelInfoItem()
{
	_pPixelInfoItem = new QGraphicsTextItem();
	_pPixelInfoItem->setPos(0, 0);
	_pPixelInfoItem->setDefaultTextColor(Qt::white);
}

void VisionLive::LiveFeedView::initAreaStatisticsItems()
{
	_pAvgItem = new QGraphicsTextItem();
	_pAvgItem->setDefaultTextColor(Qt::white);
	_pSNRItem = new QGraphicsTextItem();
	_pSNRItem->setDefaultTextColor(Qt::white);

	if(_pImageImfoItem && _pFPSInfoItem)
	{
		_pAvgItem->setPos(0, _pImageImfoItem->boundingRect().height() + _pFPSInfoItem->boundingRect().height());
		_pSNRItem->setPos(0, _pImageImfoItem->boundingRect().height() + _pFPSInfoItem->boundingRect().height() + _pAvgItem->boundingRect().height());
	}
}

void VisionLive::LiveFeedView::initAWBItems()
{
	_pAWBItem = new QGraphicsTextItem();
	_pAWBItem->setDefaultTextColor(Qt::white);

	if (_pImageImfoItem && _pFPSInfoItem)
	{
		_pAWBItem->setPos(0, _pImageImfoItem->boundingRect().height() + _pFPSInfoItem->boundingRect().height());
	}
}

//
// Public Slots
//

void VisionLive::LiveFeedView::imageReceived(QImage *pImage, int type, int format, int mosaic, int width, int height, bool awbDebugMode)
{
	if(_pImageItem && _pScene)
	{
		if(_width != width || _height != height || _type != type)
		{
			_pScene->deleteLater();

			initScene();

			initImageItem();

			initImageImfoItem();

			initFPSInfoItem();

			initContextMenu();

			initMarkerItem();

			initPixelInfoItem();

			initAreaStatisticsItems();

			initAWBItems();

			_pSelectionRect = NULL;

			_pLineViewLineItem = NULL;

			_pImageItem->setPixmap(QPixmap::fromImage(*pImage));
			fitInView(0, 0, _pScene->width(), _pScene->height(), Qt::KeepAspectRatio);
			delete pImage;

			_firstFrameReceived = false;
		}
		else
		{
			_pImageItem->setPixmap(QPixmap::fromImage(*pImage));
			delete pImage;
		}
	}
	
	_type = type;
	_format = format;
	_mosaic = mosaic;
	_width = width;
	_height = height;

	setLiveViewOption((int)_lineViewOption);

	if(_pImageImfoItem)
	{
		QString info = QString(tr("Type: ")) + ImageTypeString((VisionLive::ImageType)_type) +
						QString(tr(" Format: ")) + FormatString((ePxlFormat)_format) +
						QString(tr(" Mosaic: ")) + MosaicString((MOSAICType)_mosaic) +
						QString(tr(" Size: ")) + QString::number(_width) +
						QString(tr("x")) + QString::number(_height);
		_pImageImfoItem->setPlainText(info);
	}

	if (_pAWBItem)
	{
		QString awbInfo;
		if (awbDebugMode)
		{
			awbInfo += QString(tr("AWB DDEBUG MODE"));
			if ((ePxlFormat)_format < BAYER_RGGB_10 || (ePxlFormat)_format > BAYER_RGGB_12)
			{
				awbInfo += QString(tr(" -- Wrong format set!"));
			}
		}
		else
		{
			awbInfo = QString();
		}
		_pAWBItem->setPlainText(awbInfo);
	}

	if(!_firstFrameReceived)
	{
		if(_pScene)
		{
			if(_pImageImfoItem)
			{
				_pScene->addItem(_pImageImfoItem);
			}

			if(_pFPSInfoItem)
			{
				_pScene->addItem(_pFPSInfoItem);
			}

			if(_pMarkerItem)
			{
				_pScene->addItem(_pMarkerItem);
				_pScene->addItem(_pPixelInfoItem);
			}

			if (_pPixelInfoItem)
			{
				_pScene->addItem(_pPixelInfoItem);
			}

			if(_pAvgItem && _pSNRItem)
			{
				_pScene->addItem(_pAvgItem);
				_pScene->addItem(_pSNRItem);
			}

			if (_pAWBItem)
			{
				_pScene->addItem(_pAWBItem);
				_pAWBItem->setPos(0, 
					((_pImageImfoItem) ? _pImageImfoItem->boundingRect().height() : 0) +
					((_pFPSInfoItem) ? _pFPSInfoItem->boundingRect().height() : 0) +
					((_pPixelInfoItem) ? _pPixelInfoItem->boundingRect().height() : 0) +
					((_pAvgItem) ? _pAvgItem->boundingRect().height() : 0) +
					((_pSNRItem) ? _pSNRItem->boundingRect().height() : 0));
			}
		}

		_firstFrameReceived = true;
	}
}

void VisionLive::LiveFeedView::pixelInfoReceived(QPoint pixel, std::vector<int> pixelInfo)
{
	if(_pPixelInfoItem)
	{
		QString info = QString::number(pixel.x()) + "x" + QString::number(pixel.y()) + " - ";
		for(unsigned int i = 0; i < pixelInfo.size(); i++)
		{
			info += QString::number(pixelInfo[i]) + " ";
		}
	
		_pPixelInfoItem->setPlainText(info);

		int x = pixel.x();
		int y = pixel.y() - 20;

		if(x + _pPixelInfoItem->boundingRect().width() > _width)
		{
			x += _width - ceil(x + _pPixelInfoItem->boundingRect().width());
		}

		if(y < 0)
		{
			y += 0 - y;
		}

		_pPixelInfoItem->setPos(x, y);
	}
}

void VisionLive::LiveFeedView::areaStatisticsReceived(std::vector<double> areaStats)
{
	if(_pAvgItem && _pSNRItem && _pSelectionRect && areaStats.size() == 6)
	{
		QString avgInfo = "Avg: ";
		for(unsigned int i = 0; i < 3; i++)
		{
			avgInfo += QString::number((int)areaStats[i]) + " ";
		}
		_pAvgItem->setPlainText(avgInfo);

		QString snrInfo = "SNR: ";
		for(unsigned int i = 3; i < 6; i++)
		{
			snrInfo += QString::number(areaStats[i]) + " ";
		}
		_pSNRItem->setPlainText(snrInfo);
	}
}

void VisionLive::LiveFeedView::fpsInfoReceived(double fps)
{
	_fps = fps;

	if(_pFPSInfoItem)
	{
		QString info = QString(tr("FPS: ")) + QString::number(_fps);
		//_pFPSInfoItem->setHtml("<div style='background-color: rgba(0, 0, 0, 0.6);' >" + info + "</div>");
		_pFPSInfoItem->setPlainText(info);
	}
}