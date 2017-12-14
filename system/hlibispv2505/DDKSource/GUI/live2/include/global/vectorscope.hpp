#ifndef VECTORSCOPE_H
#define VECTORSCOPE_H

#include "customgraphicsview.hpp"

#include <qwidget.h>
#include <qmap.h>
#include <qrgb.h>

namespace VisionLive
{

class VectorScope: public CustomGraphicsView
{
	Q_OBJECT

public:
	VectorScope(QWidget *parent = 0); // Class constructor
	~VectorScope(); // Class destructor

protected:
	void resizeEvent(QResizeEvent *event); // Called on resize event

private:
	QGraphicsScene *_pScene; // Scene object
	void setUpScene();

	std::vector<QGraphicsLineItem *> _vectorscopeLines; // Contains all color lines
	QGraphicsEllipseItem *_pSaturation100Item; // 100% saturation line
	QGraphicsEllipseItem *_pSaturation60Item; // 60% saturation line
	QGraphicsEllipseItem *_pSaturation20Item; // 20% saturation line
	void setUpVectorscope(); // Initiates VectorscopeLineItem

public slots:
	void vectorscopeReceived(QMap<int, std::pair<int, QRgb> > points); // Called when ImageHandler finished calculating vectorscope data 
};

} // namespace VisionLive

#endif // VECTORSCOPE_H
