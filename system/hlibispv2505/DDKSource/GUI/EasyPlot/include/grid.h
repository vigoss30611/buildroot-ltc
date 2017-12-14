#ifndef EASYPLOT_GRID_H
#define EASYPLOT_GRID_H

#include "component.h"

namespace EasyPlot {

class Grid: public Component
{
    Q_OBJECT

public:
	Grid(int width, int height, QGraphicsItem *parent = 0);
	~Grid();
    const GridData& data() const;
    void setShow(bool option);
	void setData(const GridData &data);

protected:
    bool _show;
    float _scaleFactorX;
    float _scaleFactorY;
	GridData _data;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    void init();

};

} // namespace EasyPlot

#endif // EASYPLOT_GRID_H

