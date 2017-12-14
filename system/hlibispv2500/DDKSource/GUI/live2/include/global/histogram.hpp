#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <qwidget.h>
#include "cv.h"

class QVBoxLayout;

class QwtPlot;
class QwtPlotHistogram;
class QwtIntervalSample;
class QwtColumnSymbol;

namespace VisionLive
{

class Histogram: public QWidget
{
	Q_OBJECT

public:
	Histogram(QWidget *parent = 0); // Class constructor
	~Histogram(); // Class destructor

private:
	QVBoxLayout *_pLayout; // Main layout
	QwtPlot *_pPlotR; // Ploting area for red color
	QwtPlot *_pPlotG; // Ploting area for green color
	QwtPlot *_pPlotG2; // Ploting area for green2 (when RGGB mosaic) color
	QwtPlot *_pPlotB; // Ploting area for blue color
	QwtPlotHistogram *_pHistR; // Red color plot
	QwtPlotHistogram *_pHistG; // Green color plot
	QwtPlotHistogram *_pHistG2; // Green2 color (when RGGB mosaic) plot
	QwtPlotHistogram *_pHistB; // Blue color plot
	QwtColumnSymbol *_pSymbolR; // Histogram bar object for red color
	QwtColumnSymbol *_pSymbolG; // Histogram bar object for green color
	QwtColumnSymbol *_pSymbolG2; // Histogram bar object for green2 (when RGGB mosaic) color
	QwtColumnSymbol *_pSymbolB; // Histogram bar object for blue color
	void initHistogram(); // Initializes Historgam

	void retranslate(); // Retranslates GUI

public slots:
	void histogramReceived(std::vector<cv::Mat> histData, int nChannels, int nElem); // Updates Histogram data

};

} // namespace VisionLive

#endif // HISTOGRAM_H
