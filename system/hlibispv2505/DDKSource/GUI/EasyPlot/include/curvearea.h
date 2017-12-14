#ifndef CURVEAREA_H
#define CURVEAREA_H

#include "area.h"

class QGraphicsProxyWidget;
class QTimeLine;

namespace EasyPlot {

class ConfigWidget;

class CurveArea: public Area
{
    Q_OBJECT

public:
    CurveArea(int width, int height, ConfigWidget *configWidget, QGraphicsItem *parent = 0);

protected:
    QGraphicsProxyWidget *_pProxyWidget;
    ConfigWidget *_pConfigWidget;
    QTimeLine *_pRotationTimeLine;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    void init();
    void initConfigWidget();
    void initRotationTimeLine();

signals:
    void rotate(qreal pos);

};

} // namespace EasyPlot

#endif // CURVEAREA_H
