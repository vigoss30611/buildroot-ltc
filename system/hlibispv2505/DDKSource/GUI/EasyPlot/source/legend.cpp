#include "legend.h"

#include <QPainter>

EasyPlot::Legend::Legend(int width, int height, QGraphicsItem *parent): Component(width, height, parent)
{
}

void EasyPlot::Legend::setData(const LegendData &data)
{
    _data = data;

    int maxNameWidth = 0;
    QGraphicsTextItem *pText = new QGraphicsTextItem();
    QMap<QString, CurveData>::iterator it;
    for(it = _data.curveData.begin(); it != _data.curveData.end(); it++)
    {
        pText->setPlainText(it->name);

        if(pText->boundingRect().width() > maxNameWidth)
            maxNameWidth = pText->boundingRect().width();
    }
    delete pText;

	int maxSymbolWidth = 30;

	int maxCurveInfoWidth = maxSymbolWidth + maxNameWidth + _data.spacing + _data.padding;
	if (maxCurveInfoWidth != _width)
        _width = maxCurveInfoWidth;

    update();
}

void EasyPlot::Legend::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    //painter->setPen(_pen);
    //painter->drawRect(boundingRect());

    painter->setPen(_data.curveNamePen);

	QString line = "Line";
	int lineWidth = painter->fontMetrics().boundingRect(line).width();
	int lineHeight = painter->fontMetrics().boundingRect(line).height();

    QString point = "Points";
    int pointWidth = painter->fontMetrics().boundingRect(point).width();
    int pointHeight = painter->fontMetrics().boundingRect(point).height();

    QString bar = "Bars";
    int barWidth = painter->fontMetrics().boundingRect(bar).width();
    int barHeight = painter->fontMetrics().boundingRect(bar).height();

    int index = 0;
    QMap<QString, CurveData>::iterator it;
    for(it = _data.curveData.begin(); it != _data.curveData.end(); it++)
    {
        QString name = it->name;
        int nameWidth = painter->fontMetrics().boundingRect(name).width();
        int nameHeight = painter->fontMetrics().boundingRect(name).height();

        painter->setPen(it->pen);

		if (it->type == Line)
		{
			painter->drawText(boundingRect().width() / 2 - (lineWidth + nameWidth + _data.spacing) / 2, boundingRect().height() / 2 - _data.curveData.size()*lineHeight / 2 + lineHeight*index + lineHeight / 2, line);

			painter->setPen(_data.curveNamePen);
			painter->drawText(boundingRect().width() / 2 - (lineWidth + nameWidth + _data.spacing) / 2 + lineWidth + _data.spacing, boundingRect().height() / 2 - _data.curveData.size()*nameHeight / 2 + nameHeight*index + nameHeight / 2, name);
		}
        else if(it->type == Point)
        {
            painter->drawText(boundingRect().width()/2-(pointWidth+nameWidth+_data.spacing)/2, boundingRect().height()/2-_data.curveData.size()*pointHeight/2+pointHeight*index+pointHeight/2, point);

            painter->setPen(_data.curveNamePen);
            painter->drawText(boundingRect().width()/2-(pointWidth+nameWidth+_data.spacing)/2+pointWidth+_data.spacing, boundingRect().height()/2-_data.curveData.size()*nameHeight/2+nameHeight*index+nameHeight/2, name);
        }
        else if(it->type == Bar)
        {
            painter->drawText(boundingRect().width()/2-(barWidth+nameWidth+_data.spacing)/2, boundingRect().height()/2-_data.curveData.size()*barHeight/2+barHeight*index+barHeight/2, bar);

            painter->setPen(_data.curveNamePen);
            painter->drawText(boundingRect().width()/2-(barWidth+nameWidth+_data.spacing)/2+barWidth+_data.spacing, boundingRect().height()/2-_data.curveData.size()*nameHeight/2+nameHeight*index+nameHeight/2, name);
        }
        index++;
    }
}


