#ifndef LINEVIEW_H
#define LINEVIEW_H

#include <qgraphicsview.h>
#include <qmap.h>

namespace VisionLive
{

class LineView: public QGraphicsView
{
	Q_OBJECT

public:
	LineView(QGraphicsView *parent = 0); // Class constructor
	~LineView(); // Class destructor
	void setLineOption(int lineOption);

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
	std::vector<QGraphicsLineItem *> _bHorizLine;
	std::vector<QGraphicsLineItem *> _rVertLine;
	std::vector<QGraphicsLineItem *> _gVertLine;
	std::vector<QGraphicsLineItem *> _bVertLine;
	void initLineView(); // Initializes LineView
	void initLineVectors(int width, int height, unsigned int step); // Initializes line vectors for all colors depending on image size

signals:
	void lineViewUpdateProgress(int percent);

public slots:
	void lineViewReceived(QPoint point, QMap<QString, std::vector<std::vector<int> > > lineViewData, unsigned int step);

};

} // namespace VisionLive

#endif // LINEVIEW_H
