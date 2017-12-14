#include "curve.h"
#include "marker.h"

#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QtCore/qmath.h>

EasyPlot::Curve::Curve(int width, int height, QGraphicsItem *parent): Component(width, height, parent)
{
    _pPositionItem = NULL;

    init();
}

EasyPlot::Curve::~Curve()
{
}

const QString& EasyPlot::Curve::name() const
{
    return _data.name;
}

const EasyPlot::CurveData& EasyPlot::Curve::data() const
{
    return _data;
}

void EasyPlot::Curve::setShow(bool option)
{
    _show = option;

    for(int i = 0; i < _markers.size(); i++)
        _markers[i]->setVisible(_show);

    update();
}

void EasyPlot::Curve::setData(const CurveData &data, bool updateMarkers)
{
    bool editableChanged = (_data.editable != data.editable);

    _data = data;

    if(updateMarkers)
    {
        for(int i = 0; i < _markers.size(); i++)
            _markers[i]->setData(_data.markerData);
    }

    if(editableChanged && _data.editable)
    {
        for(int i = 0; i < _markers.size(); i++)
            delete _markers[i];

        _markers.clear();

        for(int i = 0; i < _data.data.size(); i++)
        {
            Marker *marker = new Marker(_data.markerData.size.width(), _data.markerData.size.height(), this);
            marker->setData(_data.markerData);
            QObject::connect(marker, SIGNAL(pointChanged(QPointF)), this, SLOT(pointChanged(QPointF)));
            _markers.push_back(marker);
        }
    }
    else if(editableChanged && !_data.editable)
    {
        for(int i = 0; i < _markers.size(); i++)
            delete _markers[i];

        _markers.clear();
    }

    update();
}

void EasyPlot::Curve::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    //painter->setPen(_pen);
    //painter->fillRect(boundingRect(), Qt::red);

    if(!_show) return;

    _scaleFactorX = boundingRect().width()/(_data.stats.maxX-_data.stats.minX);
    _scaleFactorY = boundingRect().height()/(_data.stats.maxY-_data.stats.minY);

    painter->setPen(_data.pen);

    if(_data.type == EasyPlot::Line && _data.data.size() >= 2)
    {
        for(int i = 0; i < _data.data.size()-1; i++)
            painter->drawLine((_data.data[i].x()+qAbs(_data.stats.minX))*_scaleFactorX, boundingRect().height()-(_data.data[i].y()+qAbs(_data.stats.minY))*_scaleFactorY,
                              (_data.data[i+1].x()+qAbs(_data.stats.minX))*_scaleFactorX, boundingRect().height()-(_data.data[i+1].y()+qAbs(_data.stats.minY))*_scaleFactorY);
    }
    else if(_data.type == EasyPlot::Point && _data.data.size())
    {
        for(int i = 0; i < _data.data.size(); i++)
            painter->drawPoint((_data.data[i].x()+qAbs(_data.stats.minX))*_scaleFactorX, boundingRect().height()-(_data.data[i].y()+qAbs(_data.stats.minY))*_scaleFactorY);
    }
    else if(_data.type == EasyPlot::Bar && _data.data.size() >= 2)
    {
		int barWidth = 0;
        for(int i = 0; i < _data.data.size()-1; i++)
        {
			barWidth = qCeil((_data.data[i + 1].x() + qAbs(_data.stats.minX))*_scaleFactorX) - qCeil((_data.data[i].x() + qAbs(_data.stats.minX))*_scaleFactorX) - _barsSpacing;
			painter->fillRect((_data.data[i].x() + qAbs(_data.stats.minX))*_scaleFactorX, boundingRect().height() - (qAbs(_data.stats.minY))*_scaleFactorY,
				qMax(barWidth, 1), (-_data.data[i].y())*_scaleFactorY, painter->pen().color());
        }
    }

    for(int i = 0; i < _markers.size(); i++)
        _markers[i]->setPos((_data.data[i].x()+qAbs(_data.stats.minX))*_scaleFactorX-_markers[i]->boundingRect().width()/2,
                            boundingRect().height()-(_data.data[i].y()+qAbs(_data.stats.minY))*_scaleFactorY-_markers[i]->boundingRect().height()/2);
}

void EasyPlot::Curve::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    if(!_pPositionItem)
    {
        _pPositionItem = new QGraphicsTextItem(QString(), this);
		_pPositionItem->setAcceptHoverEvents(false);
    }
}

void EasyPlot::Curve::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	if (_pPositionItem)
	{
		_scaleFactorX = boundingRect().width() / (_data.stats.maxX - _data.stats.minX);
		_scaleFactorY = boundingRect().height() / (_data.stats.maxY - _data.stats.minY);

		QString xText;
		float xValue = _data.stats.minX + event->pos().x() / _scaleFactorX;
		QString yText;
		float yValue = _data.stats.maxY - event->pos().y() / _scaleFactorY;

		if (xValue < EASYPLOT_FORMAT_CHANGE_VALUE)
			xText = QString::number(xValue, 'f', 2);
		else
			xText = QString::number(xValue, 'g', 3);

		if (yValue < EASYPLOT_FORMAT_CHANGE_VALUE)
			yText = QString::number(yValue, 'f', 2);
		else
			yText = QString::number(yValue, 'g', 3);

		QString posText = "(" + xText + ", " + yText + ")";

		_pPositionItem->setPlainText(posText);
		_pPositionItem->setDefaultTextColor(_data.positionInfoColor);
		_pPositionItem->setZValue(101);

		qreal x = 0.0f;
		qreal y = 0.0f;

		if (event->pos().x() + _pPositionItem->boundingRect().width() > boundingRect().width())
			x = event->pos().x() - _pPositionItem->boundingRect().width();
		else
			x = event->pos().x();

		if (event->pos().y() - _pPositionItem->boundingRect().height() > 0)
			y = event->pos().y() - _pPositionItem->boundingRect().height();
		else
			y = event->pos().y() + _pPositionItem->boundingRect().height();

		_pPositionItem->setPos(x, y);
	}
	else if (!_pPositionItem)
	{
		_pPositionItem = new QGraphicsTextItem(QString(), this);
		_pPositionItem->setAcceptHoverEvents(false);
	}
}

void EasyPlot::Curve::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    if(_pPositionItem)
    {
        delete _pPositionItem;
        _pPositionItem = NULL;
    }
}

void EasyPlot::Curve::init()
{
    _show = true;

    _scaleFactorX = 1.0;
    _scaleFactorY = 1.0;

	_barsSpacing = 1;

    setHandlesChildEvents(false);
    setAcceptHoverEvents(true);
}

void EasyPlot::Curve::pointChanged(QPointF currPos)
{
    for(int i = 0; i < _markers.size(); i++)
    {
        if(_markers[i] == sender())
        {
            emit pointChanged(_data.name, i, _data.data[i], QPointF(_data.stats.minX+currPos.x()/_scaleFactorX, _data.stats.maxY-currPos.y()/_scaleFactorY));
            break;
        }
    }
}
