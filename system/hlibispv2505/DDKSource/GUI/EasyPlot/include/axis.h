#ifndef EASYPLOT_AXIS_H
#define EASYPLOT_AXIS_H

#include "component.h"

namespace EasyPlot {

class Axis: public Component
{
public:
    Axis(int width, int height, Orientation orient, QGraphicsItem *parent = 0);
    const AxisData &data() const;
    void setData(const AxisData &data);

protected:
	Orientation _orient;
    AxisData _data;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

};

} // namespace EasyPlot

#endif // EASYPLOT_AXIS_H

