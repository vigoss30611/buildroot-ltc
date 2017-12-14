#include "grid.h"

#include <QPainter>

EasyPlot::Grid::Grid(int width, int height, QGraphicsItem *parent) : Component(width, height, parent)
{
    init();
}

EasyPlot::Grid::~Grid()
{
}

const EasyPlot::GridData& EasyPlot::Grid::data() const
{
    return _data;
}

void EasyPlot::Grid::setShow(bool option)
{
    _show = option;

    update();
}

void EasyPlot::Grid::setData(const GridData &data)
{
    _data = data;

    update();
}

void EasyPlot::Grid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    //painter->setPen(_pen);
    //painter->fillRect(boundingRect(), Qt::red);

    if(!_show) return;

	painter->setPen(_data.axisXData.gridPen);

	if (_data.axisXData.drawGrid)
	{
		for (int i = 0; i <= _data.axisXData.numMarks; i++)
			painter->drawLine((boundingRect().width() / _data.axisXData.numMarks)*i, 0, 
				(boundingRect().width() / _data.axisXData.numMarks)*i, boundingRect().height());
	}

	painter->setPen(_data.axisYData.gridPen);

	if (_data.axisYData.drawGrid)
	{
		for (int i = 0; i <= _data.axisYData.numMarks; i++)
			painter->drawLine(0, boundingRect().height() - (boundingRect().height() / _data.axisYData.numMarks)*i,
				boundingRect().width(), boundingRect().height() - (boundingRect().height() / _data.axisYData.numMarks)*i);
	}
}

void EasyPlot::Grid::init()
{
    _show = true;

    //setHandlesChildEvents(false);
}
