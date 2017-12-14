#ifndef EASYPLOT_TITLE_H
#define EASYPLOT_TITLE_H

#include "component.h"

namespace EasyPlot {

class Title: public Component
{
public:
    Title(int width, int height, Orientation orient, QGraphicsItem *parent = 0);
    ~Title();
    const TitleData &data() const;
    void setData(const TitleData &data);

protected:
	Orientation _orient;
    TitleData _data;
    QGraphicsTextItem *_pTitleItem;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    void init();
};

} // namespace EasyPlot

#endif // EASYPLOT_TITLE_H
