#ifndef EASYPLOT_LEGEND_H
#define EASYPLOT_LEGEND_H

#include "component.h"

namespace EasyPlot {

class Legend: public Component
{
public:
    Legend(int width, int height, QGraphicsItem *parent = 0);
    void setData(const LegendData &data);

protected:
    LegendData _data;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    void init();

};

} // namespace EasyPlot

#endif // EASYPLOT_LEGEND_H
