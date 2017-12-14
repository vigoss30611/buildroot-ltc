#include "area.h"

EasyPlot::Area::Area(int width, int height, QGraphicsItem *parent): QObject(), QGraphicsItemGroup(parent)
{
    _width = width;
    _height = height;

    init();
}

QRectF EasyPlot::Area::boundingRect() const
{
    return QRectF(0, 0, _width, _height);
}

void EasyPlot::Area::resize(int width, int height)
{
    _width = (width < 0)? _width : width;
    _height = (height < 0)? _height : height;

    update();
}

void EasyPlot::Area::init()
{
    setHandlesChildEvents(false);
}

