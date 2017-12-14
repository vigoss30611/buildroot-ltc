#ifndef EASYPLOT_MARKER_H
#define EASYPLOT_MARKER_H

#include "component.h"

namespace EasyPlot {

class Marker: public Component
{
    Q_OBJECT

public:
    Marker(int width,
           int height,
           QGraphicsItem *parent = 0);
    const MarkerData& data() const;
    void setData(const MarkerData &data);

protected:
    MarkerData _data;

    bool _editing;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    void init();

signals:
    void pointChanged(QPointF currPos);

};

} // namespace EasyPlot

#endif // EASYPLOT_MARKER_H
