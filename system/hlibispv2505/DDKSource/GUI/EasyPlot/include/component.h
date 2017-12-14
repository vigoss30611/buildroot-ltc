#ifndef EASYPLOT_COMPONENT_H
#define EASYPLOT_COMPONENT_H

#include <QGraphicsItem>

#include "common.h"

namespace EasyPlot {

class Component: public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    Component(int width, int height, QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void resize(int width, int height);
    void setPen(const QPen &pen);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    int _width;
    int _height;

    QPen _pen;

private:
    void init();

signals:
    void reposition();

};

} // namespace EasyPlot

#endif // EASYPLOT_COMPONENT_H
