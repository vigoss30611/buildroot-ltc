#include "title.h"

#include <QPainter>

EasyPlot::Title::Title(int width, int height, Orientation orient, QGraphicsItem *parent): Component(width, height, parent)
{
    _pTitleItem = NULL;

	_orient = orient;

    init();
}

EasyPlot::Title::~Title()
{
    if(_pTitleItem)
    {
        delete _pTitleItem;
    }
}

const EasyPlot::TitleData &EasyPlot::Title::data() const
{
    return _data;
}

void EasyPlot::Title::setData(const TitleData &data)
{
    _data = data;

	if(_pTitleItem)
	{
		_pTitleItem->setPlainText(_data.text);
		_pTitleItem->setFont(_data.font);
		_pTitleItem->setDefaultTextColor(_data.color);

		if(_orient == Vertical)
			_width = _data.font.pointSize()+20;
		else if(_orient == Horizontal)
			_height = _data.font.pointSize()+20;
	}

    update();
}

void EasyPlot::Title::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_UNUSED(painter);

    //painter->setPen(_pen);
    //painter->fillRect(boundingRect(), _pen.color());

	if(!_pTitleItem)
		return;

    if(_orient == Vertical)
        _pTitleItem->setPos(boundingRect().width()/2+_pTitleItem->boundingRect().height()/2,
                          boundingRect().height()/2-_pTitleItem->boundingRect().width()/2);
    else if(_orient == Horizontal)
        _pTitleItem->setPos(boundingRect().width()/2-_pTitleItem->boundingRect().width()/2,
                          boundingRect().height()/2-_pTitleItem->boundingRect().height()/2);
}

void EasyPlot::Title::init()
{
    if(_pTitleItem)
        delete _pTitleItem;

    _pTitleItem = new QGraphicsTextItem(QString(), this);

	if(_orient == Vertical)
        _pTitleItem->setRotation(90);
}
