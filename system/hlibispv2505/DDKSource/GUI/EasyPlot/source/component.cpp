#include "component.h"

#include <QPainter>

EasyPlot::Component::Component(int width, int height, QGraphicsItem *parent): QObject(), QGraphicsItem(parent)
{
    _width = width;
    _height = height;

    init();
}

QRectF EasyPlot::Component::boundingRect() const
{
    return QRectF(0, 0, _width, _height);
}

void EasyPlot::Component::resize(int width, int height)
{
    _width = (width < 0)? _width : width;
    _height = (height < 0)? _height : height;

    update();
}

void EasyPlot::Component::setPen(const QPen &pen)
{
    _pen = pen;

    update();
}

void EasyPlot::Component::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_UNUSED(painter);

    //painter->setPen(_pen);
    //painter->fillRect(boundingRect(), _pen.color());
}

void EasyPlot::Component::init()
{
    _pen = QPen();
}

