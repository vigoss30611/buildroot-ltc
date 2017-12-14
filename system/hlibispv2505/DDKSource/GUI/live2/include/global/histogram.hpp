#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <qwidget.h>
#include <extplot.h>

class QVBoxLayout;

namespace VisionLive
{

class Histogram: public QWidget
{
	Q_OBJECT

public:
	Histogram(QWidget *parent = 0); // Class constructor
	~Histogram(); // Class destructor
	void setPlotColors(QColor color1, QColor color2, QColor color3, QColor color4); // Uset to change graph colors based on current color space (RGB/YUV)

private:
	QVBoxLayout *_pLayout; // Main layout

	EasyPlot::ExtPlot *_pPlot; // Plot widget with view controls

	QVector<QPointF> zeroData; // Data to zero plot

	void initHistogram(); // Initializes Historgam

	void retranslate(); // Retranslates GUI

public slots:
	void histogramReceived(QVector<QVector<QPointF> > histData); // Updates Histogram data

};

} // namespace VisionLive

#endif // HISTOGRAM_H
