#ifndef LINEVIEW_H
#define LINEVIEW_H

#include "customgraphicsview.hpp"

#include <qwidget.h>
#include <qmap.h>

namespace VisionLive
{

class LineView: public CustomGraphicsView
{
	Q_OBJECT

public:
	LineView(QWidget *parent = 0); // Class constructor
	~LineView(); // Class destructor
	void setLineOption(int lineOption); // Sets graph option horizontal/vertical
	void setPlotColors(QColor color1, QColor color2, QColor color3, QColor color4); // Uset to change graph colors based on current color space (RGB/YUV)

protected:
	void resizeEvent(QResizeEvent *event); // Called on resize event

private:
	enum LineOption {Horizontal, Vertical};
	LineOption _line;
	unsigned int _step;
	QGraphicsScene *_pScene; // Scene object
	QGraphicsLineItem *_pHZeroLine;
	QGraphicsLineItem *_pVZeroLine;
	QGraphicsLineItem *_pHPositionLine;
	QGraphicsLineItem *_pVPositionLine;
	std::vector<QGraphicsLineItem *> _rHorizLine;
	std::vector<QGraphicsLineItem *> _gHorizLine;
	std::vector<QGraphicsLineItem *> _g2HorizLine;
	std::vector<QGraphicsLineItem *> _bHorizLine;
	std::vector<QGraphicsLineItem *> _rVertLine;
	std::vector<QGraphicsLineItem *> _gVertLine;
	std::vector<QGraphicsLineItem *> _g2VertLine;
	std::vector<QGraphicsLineItem *> _bVertLine;
	QColor _color1;
	QColor _color2;
	QColor _color3;
	QColor _color4;
	bool _updatePlotColors;
	void initLineView(); // Initializes LineView
	void initLineVectors(int width, int height, unsigned int step); // Initializes line vectors for all colors depending on image size

signals:
	void lineViewUpdateProgress(int percent);

public slots:
	void lineViewReceived(QPoint point, QMap<QString, std::vector<std::vector<int> > > lineViewData, unsigned int step);

};

} // namespace VisionLive

#endif // LINEVIEW_H
