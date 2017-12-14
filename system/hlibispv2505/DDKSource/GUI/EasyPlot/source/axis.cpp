#include "axis.h"

#include <qpainter.h>

EasyPlot::Axis::Axis(int width, int height, Orientation orient, QGraphicsItem *parent): Component(width, height, parent)
{
	_orient = orient;
}

const EasyPlot::AxisData &EasyPlot::Axis::data() const
{
    return _data;
}

void EasyPlot::Axis::setData(const AxisData &data)
{
    _data = data;

    update();
}

void EasyPlot::Axis::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    //painter->setPen(_pen);
    //painter->drawRect(boundingRect());

    painter->setPen(_data.pen);

    float axisMarkingSize = 0.0f;
    QString text;
	float value = 0.0;
    int textWidth = 0;
    int textHeight = 0;
	int maxContentSize = 0;

    if(_orient == Horizontal)
    {
        painter->drawLine(0, 0, boundingRect().width(), 0);

        for(int i = 0; i <= _data.numMarks; i++)
        {
            axisMarkingSize = (i%(_data.numMarks/5) == 0)? _data.maxMarkSize : (i%(_data.numMarks/10) == 0)? _data.midMarkSize : _data.minMarkSize;

            painter->drawLine((boundingRect().width()/_data.numMarks)*i, 0, (boundingRect().width()/_data.numMarks)*i, axisMarkingSize);

            if(axisMarkingSize == _data.maxMarkSize)
            {
				value = _data.stats.minX + ((_data.stats.maxX - _data.stats.minX) / _data.numMarks)*i;
				if (value < EASYPLOT_FORMAT_CHANGE_VALUE)
					text = QString::number(value, 'f', 2);
				else
					text = QString::number(value, 'g', 3);
                textWidth = painter->fontMetrics().boundingRect(text).width();
                textHeight = painter->fontMetrics().boundingRect(text).height();
                painter->drawText((boundingRect().width()/_data.numMarks)*i - textWidth/2, 
					_data.maxMarkSize + textHeight, text);

                if(_data.maxMarkSize + textHeight > maxContentSize)
                    maxContentSize = _data.maxMarkSize + textHeight;
            }
        }

		maxContentSize += 10;
        if(_height != maxContentSize)
		{
			_height = maxContentSize;
			emit reposition();
		}
    }
    else if(_orient == Vertical)
    {
        painter->drawLine(boundingRect().width(), 0, boundingRect().width(), boundingRect().height());

        for(int i = 0; i <= _data.numMarks; i++)
        {
            axisMarkingSize = (i%(_data.numMarks/5) == 0)? _data.maxMarkSize : (i%(_data.numMarks/10) == 0)? _data.midMarkSize : _data.minMarkSize;

            painter->drawLine(boundingRect().width(), boundingRect().height()-(boundingRect().height()/_data.numMarks)*i,
                              boundingRect().width()-axisMarkingSize, boundingRect().height()-(boundingRect().height()/_data.numMarks)*i);

            if(axisMarkingSize == _data.maxMarkSize)
            {
				value = _data.stats.minY + ((_data.stats.maxY - _data.stats.minY) / _data.numMarks)*i;
				if (value < EASYPLOT_FORMAT_CHANGE_VALUE)
					text = QString::number(value, 'f', 2);
				else
					text = QString::number(value, 'g', 3);
                textWidth = painter->fontMetrics().boundingRect(text).width();
                textHeight = painter->fontMetrics().boundingRect(text).height();
                painter->drawText(boundingRect().width()-2*_data.maxMarkSize-textWidth, 
					boundingRect().height()-(boundingRect().height()/_data.numMarks)*i+textHeight/2, text);

                if(2*_data.maxMarkSize + textWidth > maxContentSize)
                    maxContentSize = 2*_data.maxMarkSize + textWidth;
            }
        }

		maxContentSize += 10;
        if(_width != maxContentSize)
		{
			_width = maxContentSize;
			emit reposition();
		}
    }
}



