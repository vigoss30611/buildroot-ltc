#include "lineview.hpp"

#include <qevent.h>
#include <QGraphicsLineItem>

//
// Public Functions
//

VisionLive::LineView::LineView(QWidget *parent): CustomGraphicsView(parent)
{
	_pScene = NULL;
	_pHZeroLine = NULL;
	_pVZeroLine = NULL;
	_pHPositionLine = NULL;
	_pVPositionLine = NULL;

	_line = Horizontal;
	_step = 1;

	_color1 = Qt::red;
	_color2 = Qt::green;
	_color3 = Qt::darkGreen;
	_color4 = Qt::blue;
	_updatePlotColors = false;

	initLineView();
}

VisionLive::LineView::~LineView()
{
	if(_pScene)
	{
		_pScene->deleteLater();
	}
}

void VisionLive::LineView::setLineOption(int lineOption)
{
	_line = ((lineOption == 0)? Horizontal : Vertical);

	initLineVectors(_rHorizLine.size(), _rVertLine.size(), _step);
}

void VisionLive::LineView::setPlotColors(QColor color1, QColor color2, QColor color3, QColor color4)
{
	_color1 = color1;
	_color2 = color2;
	_color3 = color3;
	_color4 = color4;

	_updatePlotColors = true;
}

//
// Protected Functions
//

void VisionLive::LineView::resizeEvent(QResizeEvent *event)
{
	if(_pScene)
	{
		fitInView(0, 0, _pScene->width(), _pScene->height(), Qt::KeepAspectRatio);
	}
	
	event->accept();
}

//
// Private Functions
//

void VisionLive::LineView::initLineView()
{
	if(!_pScene)
	{
		_pScene = new QGraphicsScene();
		setScene(_pScene);
	}
}

void VisionLive::LineView::initLineVectors(int width, int height, unsigned int step)
{
	if(_pScene)
	{
		_pScene->deleteLater();
		_pScene = NULL;
		initLineView();

		_rVertLine.clear();
		_gVertLine.clear();
		_g2VertLine.clear();
		_bVertLine.clear();

		_rHorizLine.clear();
		_gHorizLine.clear();
		_g2HorizLine.clear();
		_bHorizLine.clear();

		if(_line == Horizontal)
		{
			for(int i = 0; i < width-1; i++)
			{
				QGraphicsLineItem *rHLine = new QGraphicsLineItem(i*step, 0, (i+1)*step, 0);
				rHLine->setPen(QPen(QBrush(_color1), 1));
				_pScene->addItem(rHLine);

				QGraphicsLineItem *gHLine = new QGraphicsLineItem(i*step, 0, (i+1)*step, 0);
				gHLine->setPen(QPen(QBrush(_color2), 1));
				_pScene->addItem(gHLine);

				QGraphicsLineItem *g2HLine = new QGraphicsLineItem(i*step, 0, (i+1)*step, 0);
				g2HLine->setPen(QPen(QBrush(_color3), 1));
				_pScene->addItem(g2HLine);

				QGraphicsLineItem *bHLine = new QGraphicsLineItem(i*step, 0, (i+1)*step, 0);
				bHLine->setPen(QPen(QBrush(_color4), 1));
				_pScene->addItem(bHLine);

				_rHorizLine.push_back(rHLine);
				_gHorizLine.push_back(gHLine);
				_g2HorizLine.push_back(g2HLine);
				_bHorizLine.push_back(bHLine);
			}

			_pHZeroLine = new QGraphicsLineItem(0, 255, width*step, 255);
			_pHZeroLine->setPen(QPen(QBrush(Qt::white), 1));
			_pScene->addItem(_pHZeroLine);

			_pHPositionLine = new QGraphicsLineItem(0, 0, 0, 0);
			_pHPositionLine->setPen(QPen(QBrush(Qt::white), 1));
			_pScene->addItem(_pHPositionLine);
		}
		else if(_line == Vertical)
		{
			for(int i = 0; i < height-1; i++)
			{
				QGraphicsLineItem *rVLine = new QGraphicsLineItem(0, i*step, 0, (i+1)*step);
				rVLine->setPen(QPen(QBrush(_color1), 1));
				_pScene->addItem(rVLine);

				QGraphicsLineItem *gVLine = new QGraphicsLineItem(0, i*step, 0, (i+1)*step);
				gVLine->setPen(QPen(QBrush(_color2), 1));
				_pScene->addItem(gVLine);

				QGraphicsLineItem *g2VLine = new QGraphicsLineItem(0, i*step, 0, (i+1)*step);
				g2VLine->setPen(QPen(QBrush(_color3), 1));
				_pScene->addItem(g2VLine);

				QGraphicsLineItem *bVLine = new QGraphicsLineItem(0, i*step, 0, (i+1)*step);
				bVLine->setPen(QPen(QBrush(_color4), 1));
				_pScene->addItem(bVLine);

				_rVertLine.push_back(rVLine);
				_gVertLine.push_back(gVLine);
				_g2VertLine.push_back(g2VLine);
				_bVertLine.push_back(bVLine);
			}

			_pVZeroLine = new QGraphicsLineItem(0, 0, 0, height*step);
			_pVZeroLine->setPen(QPen(QBrush(Qt::white), 1));
			_pScene->addItem(_pVZeroLine);

			_pVPositionLine = new QGraphicsLineItem(0, 0, 0, 0);
			_pVPositionLine->setPen(QPen(QBrush(Qt::white), 1));
			_pScene->addItem(_pVPositionLine);
		}

		fitInView(0, 0, _pScene->width(), _pScene->height(), Qt::KeepAspectRatio);

		_updatePlotColors = false;
	}
}

//
// Public Slots
//

void VisionLive::LineView::lineViewReceived(QPoint point, QMap<QString, std::vector<std::vector<int> > > lineViewData, unsigned int step)
{
	_step = step;

	std::vector<std::vector<int> > horizLine = lineViewData.find("Horizontal").value();
	std::vector<std::vector<int> > vertLine = lineViewData.find("Vertical").value();

	if(_line == Horizontal && horizLine.size() > 0)
	{
		if(horizLine[0].size()-1 != _bHorizLine.size() || _updatePlotColors)
		{
			initLineVectors(horizLine[0].size(), vertLine[0].size(), step);
		}

		for(unsigned int i = 0; i < horizLine[0].size()-1; i++)
		{
			_rHorizLine[i]->setLine(i*step, 255-horizLine[0][i], (i+1)*step, 255-horizLine[0][i+1]);
			_gHorizLine[i]->setLine(i*step, 255-horizLine[1][i], (i+1)*step, 255-horizLine[1][i+1]);
			_g2HorizLine[i]->setLine(i*step, 255-horizLine[2][i], (i+1)*step, 255-horizLine[2][i+1]);
			_bHorizLine[i]->setLine(i*step, 255-horizLine[3][i], (i+1)*step, 255-horizLine[3][i+1]);

			emit lineViewUpdateProgress(i*100/(horizLine[0].size()-1-1));
		}

		_pHPositionLine->setLine(point.x(), 0, point.x(), 255);
	}
	else if(_line == Vertical && vertLine.size() > 0)
	{
		if(vertLine[0].size()-1 != _bVertLine.size() || _updatePlotColors)
		{
			initLineVectors(horizLine[0].size(), vertLine[0].size(), step);
		}

		for(unsigned int i = 0; i < vertLine[0].size()-1; i++)
		{
			_rVertLine[i]->setLine(vertLine[0][i], i*step, vertLine[0][i+1], (i+1)*step);
			_gVertLine[i]->setLine(vertLine[1][i], i*step, vertLine[1][i+1], (i+1)*step);
			_g2VertLine[i]->setLine(vertLine[2][i], i*step, vertLine[2][i+1], (i+1)*step);
			_bVertLine[i]->setLine(vertLine[3][i], i*step, vertLine[3][i+1], (i+1)*step);

			emit lineViewUpdateProgress(i*100/(vertLine[0].size()-1-1));
		}

		_pVPositionLine->setLine(0, point.y(), 255, point.y());
	}
}


