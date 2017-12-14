#include "marker.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

EasyPlot::Marker::Marker(int width, int height, QGraphicsItem *parent): Component(width, height, (QGraphicsItem *)parent)
{
    init();
}

const EasyPlot::MarkerData& EasyPlot::Marker::data() const
{
    return _data;
}

void EasyPlot::Marker::setData(const MarkerData &data)
{
    _data = data;

    _width = _data.size.width();
    _height = _data.size.height();

    update();
}

void EasyPlot::Marker::init()
{
    _editing = false;

    setAcceptHoverEvents(true);
}

void EasyPlot::Marker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen((_editing)? _data.editPen : _data.pen);
    painter->setBrush((_editing)? _data.editPen.color(): _data.pen.color());

    switch(_data.type)
    {
        case EasyPlot::Rectangle:
            painter->drawRect(0, 0, _width, _height);
            break;
        case EasyPlot::Ellipse:
            painter->drawEllipse(0, 0, _width, _height);
            break;
    }
}

void EasyPlot::Marker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    _editing = true;

    update();
}

void EasyPlot::Marker::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit pointChanged(mapToItem(this->parentItem(), event->pos()));
}

void EasyPlot::Marker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    _editing = false;

    update();
}
