#ifndef EASYPLOT_AREA_H
#define EASYPLOT_AREA_H

#include "component.h"
#include <QGraphicsItemGroup>

#include "common.h"

namespace EasyPlot {

class Area: public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:
    Area(int width, int height, QGraphicsItem *parent = 0);
    QRectF boundingRect() const;
    void resize(int width, int height);

protected:
    int _width;
    int _height;

private:
    void init();

};

} // namespace EasyPlot

#endif // EASYPLOT_AREA_H
