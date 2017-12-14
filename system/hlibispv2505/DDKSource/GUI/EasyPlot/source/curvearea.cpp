#include "curvearea.h"
#include "configwidget.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsProxyWidget>
#include <QTimeLine>

EasyPlot::CurveArea::CurveArea(int width, int height, ConfigWidget *configWidget, QGraphicsItem *parent): Area(width, height, parent)
{
    _pProxyWidget = NULL;
    _pConfigWidget = configWidget;
    _pRotationTimeLine = NULL;

    init();
}

void EasyPlot::CurveArea::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::RightButton && _pRotationTimeLine)
        _pRotationTimeLine->start();
}

void EasyPlot::CurveArea::init()
{
    initConfigWidget();
    initRotationTimeLine();
}

void EasyPlot::CurveArea::initConfigWidget()
{
    if(!_pConfigWidget)
        return;

    _pConfigWidget->setVisible(false);

    _pProxyWidget = new QGraphicsProxyWidget(this);
    _pProxyWidget->setWidget(_pConfigWidget);
    addToGroup(_pProxyWidget);
}

void EasyPlot::CurveArea::initRotationTimeLine()
{
    if(_pRotationTimeLine)
        delete _pRotationTimeLine;

    _pRotationTimeLine = new QTimeLine(1000, this);
    _pRotationTimeLine->setCurveShape(QTimeLine::EaseInOutCurve);
    QObject::connect(_pRotationTimeLine, SIGNAL(valueChanged(qreal)), this, SIGNAL(rotate(qreal)));
}

