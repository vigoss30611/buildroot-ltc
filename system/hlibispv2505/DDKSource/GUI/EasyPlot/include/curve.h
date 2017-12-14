#ifndef EASYPLOT_CURVE_H
#define EASYPLOT_CURVE_H

#include "component.h"

namespace EasyPlot {

class Marker;

class Curve: public Component
{
    Q_OBJECT

public:
    Curve(int width, int height, QGraphicsItem *parent = 0);
    ~Curve();
    const QString& name() const;
    const CurveData& data() const;
    void setShow(bool option);
    void setData(const CurveData &data, bool updateMarkers = true);

protected:
    bool _show;
    float _scaleFactorX;
    float _scaleFactorY;
	int _barsSpacing;
    CurveData _data;
    QVector<Marker *> _markers;

    QGraphicsTextItem *_pPositionItem;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    void init();

signals:
    void pointChanged(QString name, unsigned index, QPointF prevPos, QPointF currPos);

public slots:
    void pointChanged(QPointF currPos);

};

} // namespace EasyPlot

#endif // EASYPLOT_CURVE_H

